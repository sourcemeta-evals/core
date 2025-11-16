class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In modern JSON Schema versions (2019-09 and later), `$ref` no "
            "longer overrides sibling keywords, making `allOf` wrappers around "
            "a single `$ref` unnecessary"} {};

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

    if (!schema.is_object() || !schema.defines("allOf") ||
        schema.defines("$ref")) {
      return false;
    }

    const auto &allof_value = schema.at("allOf");
    if (!allof_value.is_array() || allof_value.size() != 1) {
      return false;
    }

    const auto &single_branch = allof_value.at(0);
    if (!single_branch.is_object() || !single_branch.defines("$ref")) {
      return false;
    }

    return single_branch.size() == 1;
  }

  auto transform(JSON &schema) const -> void override {
    const auto &allof_value = schema.at("allOf");
    const auto &single_branch = allof_value.at(0);
    const auto ref_value = single_branch.at("$ref");

    schema.erase("allOf");
    schema.assign("$ref", ref_value);
  }
};
