class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` no longer overrides "
            "sibling keywords, so wrapping a single `$ref` in `allOf` is "
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
    if (!contains_any(vocabularies,
                      {"https://json-schema.org/draft/2020-12/vocab/core",
                       "https://json-schema.org/draft/2019-09/vocab/core"})) {
      return false;
    }

    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    if (schema.defines("$ref")) {
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
    auto allof = schema.at("allOf");
    auto branch = allof.front();
    auto ref = branch.at("$ref");
    schema.erase("allOf");
    schema.assign("$ref", std::move(ref));
  }
};
