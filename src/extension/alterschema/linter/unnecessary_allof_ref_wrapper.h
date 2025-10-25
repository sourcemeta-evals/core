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
    // Only applies to 2019-09 and later where $ref doesn't override siblings
    if (!contains_any(
            vocabularies,
            {"https://json-schema.org/draft/2020-12/vocab/applicator",
             "https://json-schema.org/draft/2019-09/vocab/applicator"})) {
      return false;
    }

    // Must have allOf as an array with exactly one element
    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array() || schema.at("allOf").size() != 1) {
      return false;
    }

    // The single element must be an object with $ref
    const auto &allof_element = schema.at("allOf").at(0);
    if (!allof_element.is_object() || !allof_element.defines("$ref")) {
      return false;
    }

    // There must be no sibling keywords to allOf (except $schema, $id, etc.)
    // that would be lost if we extract the $ref
    // We check that there are no other keywords besides allOf at the schema
    // level Actually, we should allow other keywords since in 2019-09+ they
    // work with $ref So we just need to ensure the allOf has exactly one branch
    // with a $ref
    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the single allOf element
    auto allof_element = schema.at("allOf").at(0);

    // Remove the allOf keyword
    schema.erase("allOf");

    // Merge all properties from the allOf element into the schema
    for (auto &property : allof_element.as_object()) {
      schema.assign(property.first, std::move(property.second));
    }
  }
};
