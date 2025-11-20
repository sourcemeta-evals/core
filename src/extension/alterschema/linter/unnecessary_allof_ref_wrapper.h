class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In modern JSON Schema versions (2019-09 and later), wrapping a "
            "`$ref` in `allOf` is unnecessary as `$ref` no longer overrides "
            "sibling keywords"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    // Only apply to modern JSON Schema versions (2019-09 and later)
    if (!contains_any(vocabularies,
                      {"https://json-schema.org/draft/2020-12/vocab/core",
                       "https://json-schema.org/draft/2019-09/vocab/core"})) {
      return false;
    }

    // Schema must be an object with allOf
    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    // Schema must have only $schema and allOf keywords (no other siblings)
    // Check if there are any keywords other than $schema and allOf
    if (schema.size() > 2 ||
        (schema.size() == 2 && !schema.defines("$schema"))) {
      return false;
    }

    const auto &allof = schema.at("allOf");

    // allOf must be an array
    if (!allof.is_array()) {
      return false;
    }

    // Only apply if there's exactly one branch with $ref and it's the only
    // branch
    if (allof.size() == 1) {
      const auto &branch = allof.at(0);
      if (branch.is_object() && branch.defines("$ref")) {
        // Check if the branch contains only $ref (no other keywords)
        if (branch.size() == 1) {
          return true;
        }
      }
    }

    return false;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the $ref from the single allOf branch
    auto ref_value = schema.at("allOf").at(0).at("$ref");

    // Remove the allOf keyword
    schema.erase("allOf");

    // Add the $ref at the schema level
    schema.assign("$ref", std::move(ref_value));
  }
};
