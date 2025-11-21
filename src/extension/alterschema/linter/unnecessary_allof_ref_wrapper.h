class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a `$ref` in `allOf` is unnecessary in modern JSON Schema "
            "versions (2019-09 and later) where `$ref` no longer overrides "
            "sibling keywords"} {};

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
    if (!contains_any(vocabularies,
                      {"https://json-schema.org/draft/2020-12/vocab/core",
                       "https://json-schema.org/draft/2019-09/vocab/core"})) {
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

    // The single element must be an object with $ref
    const auto &element = allof.front();
    if (!element.is_object() || !element.defines("$ref")) {
      return false;
    }

    // There must be no $ref sibling to allOf (otherwise we can't extract)
    if (schema.defines("$ref")) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the single allOf element and merge it into the parent schema
    auto allof_element = schema.at("allOf").front();

    // Remove the allOf keyword
    schema.erase("allOf");

    // Merge all properties from the allOf element into the parent schema
    for (const auto &pair : allof_element.as_object()) {
      schema.assign(pair.first, pair.second);
    }
  }
};
