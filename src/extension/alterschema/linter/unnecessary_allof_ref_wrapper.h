class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In modern JSON Schema (2019-09 and later), wrapping a single "
            "`$ref` in `allOf` is unnecessary since `$ref` no longer overrides "
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
    if (!contains_any(vocabularies,
                      {"https://json-schema.org/draft/2020-12/vocab/core",
                       "https://json-schema.org/draft/2019-09/vocab/core"})) {
      return false;
    }

    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.size() != 1) {
      return false;
    }

    const auto &branch = allof.at(0);
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    // Check if the branch has only $ref and no other keywords
    if (branch.size() != 1) {
      return false;
    }

    // Check if there are any sibling keywords to allOf (other than $schema)
    // Count non-$schema keywords
    std::size_t non_schema_keywords = 0;
    for (const auto &entry : schema.as_object()) {
      if (entry.first != "$schema") {
        non_schema_keywords++;
      }
    }

    // If there's more than just allOf, we can't safely extract
    if (non_schema_keywords > 1) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    auto ref_value = schema.at("allOf").at(0).at("$ref");
    schema.erase("allOf");
    schema.assign("$ref", std::move(ref_value));
  }
};
