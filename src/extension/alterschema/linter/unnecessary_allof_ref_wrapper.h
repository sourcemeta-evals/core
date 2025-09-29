class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a single `$ref` in `allOf` is unnecessary in modern JSON "
            "Schema versions (2019-09 and later) where `$ref` no longer "
            "overrides sibling keywords"} {};

  [[nodiscard]] auto
  condition(const JSON &schema, const JSON &, const Vocabularies &vocabularies,
            const SchemaFrame &, const SchemaFrame::Location &,
            const SchemaWalker &, const SchemaResolver &) const
      -> Result override {
    // Only apply to modern JSON Schema versions (2019-09 and later)
    if (!contains_any(
            vocabularies,
            {"https://json-schema.org/draft/2020-12/vocab/applicator",
             "https://json-schema.org/draft/2019-09/vocab/applicator"})) {
      return false;
    }

    // Must be an object with allOf
    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.size() != 1) {
      return false;
    }

    // The single branch must contain $ref
    const auto &branch = allof.at(0);
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    // There should be no $ref sibling to allOf
    return !schema.defines("$ref");
  }

  auto transform(JSON &schema) const -> void override {
    const auto &allof = schema.at("allOf");
    const auto &ref_branch = allof.at(0);

    // Extract the $ref from the allOf wrapper
    schema.assign("$ref", ref_branch.at("$ref"));

    // Copy any other properties from the ref branch (except $ref)
    for (const auto &entry : ref_branch.as_object()) {
      if (entry.first != "$ref") {
        schema.assign(entry.first, entry.second);
      }
    }

    // Remove the allOf
    schema.erase("allOf");
  }
};
