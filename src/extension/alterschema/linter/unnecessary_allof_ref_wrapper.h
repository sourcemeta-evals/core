class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` no longer overrides "
            "sibling keywords, so wrapping a `$ref` in `allOf` is unnecessary "
            "when there is only one `$ref` among the branches"} {};

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
        !schema.at("allOf").is_array() || schema.at("allOf").empty()) {
      return false;
    }

    if (schema.defines("$ref")) {
      return false;
    }

    const auto &allof = schema.at("allOf");

    // Only apply when there's exactly one branch in allOf
    // and that branch contains ONLY $ref
    if (allof.size() != 1) {
      return false;
    }

    const auto &branch = allof.at(0);
    if (!branch.is_object() || !branch.defines("$ref") || branch.size() != 1) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema, const Result &) const -> void override {
    // We only apply when there's exactly one branch with only $ref
    auto ref_value = schema.at("allOf").at(0).at("$ref");
    // Use try_assign_before to place $ref where allOf was for proper ordering
    schema.try_assign_before("$ref", std::move(ref_value), "allOf");
    schema.erase("allOf");
  }
};
