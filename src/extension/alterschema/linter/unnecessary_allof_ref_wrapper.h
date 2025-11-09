class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` no longer overrides "
            "sibling keywords, making `allOf` wrappers around a single `$ref` "
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
    // Only applies to 2019-09 and later (where $ref doesn't override siblings)
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

    // allOf must be an array with exactly one element
    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.size() != 1) {
      return false;
    }

    // The single element must be an object with $ref
    const auto &element = allof.front();
    if (!element.is_object() || !element.defines("$ref")) {
      return false;
    }

    // The element must ONLY contain $ref (no sibling keywords)
    if (element.size() != 1) {
      return false;
    }

    // The schema itself must not have $ref as a sibling to allOf
    // (if it does, we can't safely extract)
    if (schema.defines("$ref")) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the $ref from the allOf wrapper
    auto ref_value = schema.at("allOf").front().at("$ref");

    // Remove the allOf keyword
    schema.erase("allOf");

    // Add the $ref at the top level
    schema.assign("$ref", std::move(ref_value));
  }
};
