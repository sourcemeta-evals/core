class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a `$ref` in `allOf` is unnecessary in modern JSON Schema "
            "versions where `$ref` no longer overrides sibling keywords"} {};

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
        !schema.at("allOf").is_array() || schema.at("allOf").empty()) {
      return false;
    }

    // Don't apply if there is already a $ref sibling to allOf
    if (schema.defines("$ref")) {
      return false;
    }

    // Count how many branches in allOf contain $ref
    std::size_t ref_count{0};
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref") && branch.size() == 1) {
        ref_count++;
      }
    }

    // Only apply if exactly one branch contains a sole $ref
    return ref_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    // Find the index of the branch with sole $ref
    const auto &allof = schema.at("allOf");
    std::size_t ref_index{0};
    for (std::size_t index = 0; index < allof.size(); index++) {
      const auto &branch = allof.at(index);
      if (branch.is_object() && branch.defines("$ref") && branch.size() == 1) {
        schema.assign("$ref", branch.at("$ref"));
        ref_index = index;
        break;
      }
    }

    // Remove the $ref branch from allOf by index
    schema.at("allOf").erase(std::next(schema.at("allOf").as_array().cbegin(),
                                       static_cast<std::ptrdiff_t>(ref_index)));

    if (schema.at("allOf").empty()) {
      schema.erase("allOf");
    }
  }
};
