class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Unnecessary `allOf` wrapper around `$ref` in modern JSON Schema "
            "versions (2019-09+) where `$ref` no longer overrides sibling "
            "keywords"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    return contains_any(
               vocabularies,
               {"https://json-schema.org/draft/2020-12/vocab/applicator",
                "https://json-schema.org/draft/2019-09/vocab/applicator"}) &&
           schema.is_object() && schema.defines("allOf") &&
           schema.at("allOf").is_array() && schema.at("allOf").size() == 1 &&
           schema.at("allOf").at(0).is_object() &&
           schema.at("allOf").at(0).defines("$ref") && !schema.defines("$ref");
  }

  auto transform(JSON &schema) const -> void override {
    auto ref_schema = schema.at("allOf").at(0);

    schema.erase("allOf");

    for (const auto &entry : ref_schema.as_object()) {
      schema.assign(entry.first, entry.second);
    }
  }
};
