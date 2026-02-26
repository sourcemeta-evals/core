class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In modern JSON Schema versions (2019-09 and later), `$ref` no "
            "longer overrides sibling keywords, making it unnecessary to wrap "
            "`$ref` in `allOf`"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    if (!contains_any(
            vocabularies,
            {"https://json-schema.org/draft/2020-12/vocab/applicator",
             "https://json-schema.org/draft/2019-09/vocab/applicator"})) {
      return false;
    }

    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array() || schema.defines("$ref")) {
      return false;
    }

    // Count branches that contain $ref
    std::size_t ref_count = 0;
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
      }
    }

    // Must have exactly one branch with $ref
    return ref_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    // Find the branch with $ref and extract its $ref value
    auto &array = schema.at("allOf").as_array();
    auto iterator = array.begin();
    for (; iterator != array.end(); ++iterator) {
      if (iterator->is_object() && iterator->defines("$ref")) {
        break;
      }
    }

    // Get the $ref value
    auto ref_value = iterator->at("$ref");

    // Remove the $ref branch from allOf
    schema.at("allOf").erase(iterator);

    // If allOf is now empty, remove it entirely
    if (schema.at("allOf").empty()) {
      schema.erase("allOf");
    }

    // Set $ref on the parent schema
    schema.assign("$ref", std::move(ref_value));
  }
};
