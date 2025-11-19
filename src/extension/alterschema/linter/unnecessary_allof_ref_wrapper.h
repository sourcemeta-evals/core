class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` no longer overrides "
            "sibling keywords, making single-branch `allOf` wrappers around "
            "`$ref` unnecessary"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    return contains_any(vocabularies,
                        {"https://json-schema.org/draft/2020-12/vocab/core",
                         "https://json-schema.org/draft/2019-09/vocab/core"}) &&
           schema.is_object() && schema.defines("allOf") &&
           !schema.defines("$ref") && schema.at("allOf").is_array() &&
           schema.at("allOf").size() == 1 &&
           schema.at("allOf").at(0).is_object() &&
           schema.at("allOf").at(0).defines("$ref") &&
           schema.at("allOf").at(0).size() == 1;
  }

  auto transform(JSON &schema, const Result &) const -> void override {
    auto ref_value = schema.at("allOf").at(0).at("$ref");
    schema.erase("allOf");
    schema.assign("$ref", std::move(ref_value));
  }
};
