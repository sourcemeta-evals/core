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

    // Must have allOf as an array
    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array()) {
      return false;
    }

    // Can't extract if there's already a $ref sibling to allOf
    if (schema.defines("$ref")) {
      return false;
    }

    const auto &allof_array = schema.at("allOf");

    // Only apply if allOf has exactly one branch
    if (allof_array.size() != 1) {
      return false;
    }

    // Check if that single branch contains only $ref (no other properties)
    const auto &branch = allof_array.front();
    return branch.is_object() && branch.defines("$ref") && branch.size() == 1;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the $ref from the single allOf branch
    const auto &branch = schema.at("allOf").front();
    schema.assign("$ref", branch.at("$ref"));

    // Remove the allOf wrapper entirely
    schema.erase("allOf");
  }
};
