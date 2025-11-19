class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Since Draft 2019-09, `$ref` does not override sibling keywords, "
            "so wrapping a single `$ref` in `allOf` is unnecessary"} {};

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

    // Must have allOf as an array
    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array()) {
      return false;
    }

    const auto &allof_array = schema.at("allOf");

    // allOf must have exactly one element
    if (allof_array.size() != 1) {
      return false;
    }

    // That single element must be an object with $ref
    const auto &single_element = allof_array.front();
    if (!single_element.is_object() || !single_element.defines("$ref")) {
      return false;
    }

    // The schema must not have a sibling $ref (that would be invalid anyway)
    if (schema.defines("$ref")) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the single element from allOf
    auto ref_schema = schema.at("allOf").front();

    // Remove the allOf keyword
    schema.erase("allOf");

    // Merge the $ref into the parent schema
    // We need to copy all properties from ref_schema to schema
    for (const auto &entry : ref_schema.as_object()) {
      schema.assign(entry.first, entry.second);
    }
  }
};
