class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In modern JSON Schema versions (2019-09 and later), `$ref` no "
            "longer overrides sibling keywords, so wrapping a single `$ref` "
            "in `allOf` is unnecessary"} {};

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

    // Must be an object with allOf
    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    const auto &allof = schema.at("allOf");

    // allOf must be an array
    if (!allof.is_array()) {
      return false;
    }

    // allOf must have exactly one element
    if (allof.size() != 1) {
      return false;
    }

    const auto &branch = allof.at(0);

    // The single branch must be an object with $ref
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    // The single branch must ONLY contain $ref (no sibling keywords)
    if (branch.size() != 1) {
      return false;
    }

    // The schema must not have $ref as a sibling to allOf
    // (otherwise we can't safely extract)
    if (schema.defines("$ref")) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the $ref from the single allOf branch
    auto ref_value = schema.at("allOf").at(0).at("$ref");

    // Remove the allOf keyword
    schema.erase("allOf");

    // Add the $ref directly to the schema
    schema.assign("$ref", std::move(ref_value));
  }
};
