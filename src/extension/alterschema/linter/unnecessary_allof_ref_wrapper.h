class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "An `allOf` with a single `$ref` branch and no sibling keywords "
            "can be simplified by extracting the `$ref` to the parent level "
            "in JSON Schema 2019-09 and later"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    // Only applies to JSON Schema 2019-09 and later where $ref doesn't override
    // siblings
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

    // allOf must be an array with exactly one element
    if (!allof.is_array() || allof.size() != 1) {
      return false;
    }

    const auto &branch = allof.front();

    // The single branch must be an object with only $ref
    if (!branch.is_object() || !branch.defines("$ref") || branch.size() != 1) {
      return false;
    }

    // The parent schema must not have $ref (to avoid conflicts)
    if (schema.defines("$ref")) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the $ref from the single allOf branch
    const auto ref_value = schema.at("allOf").front().at("$ref");

    // Remove the allOf
    schema.erase("allOf");

    // Add the $ref to the parent level
    schema.assign("$ref", ref_value);
  }
};
