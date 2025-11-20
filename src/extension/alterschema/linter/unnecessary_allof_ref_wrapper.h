class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "As of JSON Schema 2019-09, `$ref` no longer overrides sibling "
            "keywords, making `allOf` wrappers around single `$ref` instances "
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
    // Only apply for modern JSON Schema versions (2019-09 and later)
    if (!contains_any(vocabularies,
                      {"https://json-schema.org/draft/2020-12/vocab/core",
                       "https://json-schema.org/draft/2019-09/vocab/core"})) {
      return false;
    }

    // Check if schema has allOf
    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    // Check if allOf is an array with exactly one element
    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.size() != 1) {
      return false;
    }

    // Check if the single element has ONLY a $ref (no other properties)
    // This makes the rule more specific and avoids overlap with
    // UnnecessaryAllOfWrapperModern which handles more general cases
    const auto &element = allof.at(0);
    if (!element.is_object() || !element.defines("$ref") || element.size() != 1) {
      return false;
    }

    // Check if there's no $ref sibling to allOf at the parent level
    if (schema.defines("$ref")) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema, const Result &) const -> void override {
    // Extract the single allOf element
    const auto &allof_element = schema.at("allOf").at(0);

    // Copy all properties from the allOf element to the parent schema
    for (const auto &entry : allof_element.as_object()) {
      schema.assign(entry.first, entry.second);
    }

    // Remove the allOf keyword
    schema.erase("allOf");
  }
};
