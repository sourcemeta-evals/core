class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a single `$ref` in `allOf` is unnecessary in 2019-09 and "
            "later, as `$ref` no longer overrides sibling keywords"} {};

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

    // Check if there are no sibling keywords to allOf
    // We need to count keywords excluding $schema (metadata keyword)
    std::size_t keyword_count = 0;
    for (const auto &pair : schema.as_object()) {
      if (pair.first != "$schema") {
        keyword_count++;
      }
    }

    // If there are siblings to allOf, we can't safely extract
    if (keyword_count > 1) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    auto ref_schema = schema.at("allOf").at(0);
    schema.erase("allOf");

    // Copy all properties from the ref schema to the parent
    for (const auto &pair : ref_schema.as_object()) {
      schema.assign(pair.first, pair.second);
    }
  }
};
