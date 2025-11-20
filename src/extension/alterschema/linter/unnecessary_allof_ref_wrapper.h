class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` can have sibling "
            "keywords, so wrapping it in `allOf` is no longer necessary"} {};

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
        !schema.at("allOf").is_array() || schema.defines("$ref")) {
      return false;
    }

    const auto &all_of = schema.at("allOf");

    // Only apply when allOf has exactly one branch that is a pure $ref
    // Let UnnecessaryAllOfWrapperModern handle multi-branch cases
    if (all_of.size() != 1) {
      return false;
    }

    const auto &branch = all_of.at(0);
    return branch.is_object() && branch.defines("$ref") && branch.size() == 1;
  }

  auto transform(JSON &schema, const Result &) const -> void override {
    auto ref_value = schema.at("allOf").at(0).at("$ref");
    schema.try_assign_before("$ref", std::move(ref_value), "allOf");
    schema.erase("allOf");
  }
};
