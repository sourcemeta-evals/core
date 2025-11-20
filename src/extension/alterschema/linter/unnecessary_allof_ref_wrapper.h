class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` no longer overrides "
            "sibling keywords, making `allOf` wrappers around a single `$ref` "
            "unnecessary"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    // Only applies to 2019-09 and later (where $ref doesn't override siblings)
    if (!contains_any(
            vocabularies,
            {"https://json-schema.org/draft/2020-12/vocab/applicator",
             "https://json-schema.org/draft/2019-09/vocab/applicator"})) {
      return false;
    }

    // Must have allOf as an array
    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array()) {
      return false;
    }

    const auto &allof_array = schema.at("allOf");

    // allOf must have exactly one element
    if (allof_array.size() != 1) {
      return false;
    }

    // That element must be an object with $ref
    const auto &element = allof_array.front();
    if (!element.is_object() || !element.defines("$ref")) {
      return false;
    }

    // The schema itself must not have $ref (to avoid conflicts)
    if (schema.defines("$ref")) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the single element from allOf
    auto ref_schema = schema.at("allOf").front();

    // Remove the allOf keyword
    schema.erase("allOf");

    // Move the $ref to the parent schema
    auto ref_value = ref_schema.at("$ref");
    schema.assign("$ref", std::move(ref_value));

    // Copy any other properties from the ref_schema to the parent
    if (ref_schema.is_object()) {
      for (const auto &key : ref_schema.as_object()) {
        if (key.first != "$ref" && !schema.defines(key.first)) {
          schema.assign(key.first, ref_schema.at(key.first));
        }
      }
    }
  }
};
