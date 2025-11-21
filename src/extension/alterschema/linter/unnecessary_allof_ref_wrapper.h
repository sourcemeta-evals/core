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
    // Only applies to modern JSON Schema versions (2019-09 and later)
    if (!contains_any(
            vocabularies,
            {"https://json-schema.org/draft/2020-12/vocab/applicator",
             "https://json-schema.org/draft/2019-09/vocab/applicator"})) {
      return false;
    }

    // Must have allOf keyword
    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.size() != 1) {
      return false;
    }

    // The single branch must contain a $ref
    const auto &branch = allof.at(0);
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    // The schema must not have a sibling $ref (which would be invalid anyway)
    if (schema.defines("$ref")) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the single branch from allOf
    auto branch = schema.at("allOf").at(0);

    // Remove the allOf keyword
    schema.erase("allOf");

    // Move all properties from the branch to the parent schema
    for (auto &property : branch.as_object()) {
      schema.assign(property.first, std::move(property.second));
    }
  }
};
