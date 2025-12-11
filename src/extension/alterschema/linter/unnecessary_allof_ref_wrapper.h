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

    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array() || schema.at("allOf").empty()) {
      return false;
    }

    if (schema.defines("$ref")) {
      return false;
    }

    const auto &all_of = schema.at("allOf");

    // Only apply when allOf has exactly one branch containing only $ref
    // For multi-branch allOf, let unnecessary_allof_wrapper_modern handle it
    if (all_of.size() != 1) {
      return false;
    }

    const auto &branch = all_of.at(0);
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    // The branch must contain ONLY $ref (no other keywords)
    return branch.size() == 1;
  }

  auto transform(JSON &schema, const Result &) const -> void override {
    // We only handle single-branch allOf with only $ref
    auto ref_value = schema.at("allOf").at(0).at("$ref");

    // Use try_assign_before to place $ref before allOf in keyword order
    // (consistent with unnecessary_allof_wrapper_modern)
    schema.try_assign_before("$ref", std::move(ref_value), "allOf");

    // Remove the allOf entirely since it only had one branch
    schema.erase("allOf");
  }
};
