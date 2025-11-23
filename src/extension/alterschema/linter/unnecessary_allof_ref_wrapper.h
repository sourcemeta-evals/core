class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` no longer overrides "
            "sibling keywords, making `allOf` wrappers around single `$ref` "
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
    // Only applies to 2019-09 and later
    if (!contains_any(vocabularies,
                      {"https://json-schema.org/draft/2020-12/vocab/core",
                       "https://json-schema.org/draft/2019-09/vocab/core"})) {
      return false;
    }

    // Must have allOf
    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.size() != 1) {
      return false;
    }

    // The single branch must be an object with only $ref
    const auto &branch = allof.at(0);
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    // Check that the branch has only $ref (no sibling keywords)
    if (branch.size() != 1) {
      return false;
    }

    // Check that allOf has no sibling $ref at the parent level
    if (schema.defines("$ref")) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema, const Result &) const -> void override {
    // Extract the $ref from the allOf branch
    auto ref_value = schema.at("allOf").at(0).at("$ref");
    
    // Add the $ref to the parent schema before allOf (to preserve ordering)
    schema.try_assign_before("$ref", std::move(ref_value), "allOf");
    
    // Remove the allOf keyword
    schema.erase("allOf");
  }
};
