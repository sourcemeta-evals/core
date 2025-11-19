class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a single `$ref` in `allOf` is unnecessary in modern JSON "
            "Schema versions (2019-09 and later) where `$ref` no longer "
            "overrides sibling keywords"} {};

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
        schema.defines("$ref")) {
      return false;
    }

    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.size() != 1) {
      return false;
    }

    const auto &branch = allof.front();
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    return branch.size() == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto ref_value = schema.at("allOf").front().at("$ref");
    schema.erase("allOf");
    schema.assign("$ref", std::move(ref_value));
  }
};
