class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Unnecessary `allOf` wrapper around `$ref` in modern JSON Schema "
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

    // Must have allOf as an array with exactly one element
    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array() || schema.at("allOf").size() != 1) {
      return false;
    }

    // Must not have a sibling $ref keyword (would create conflict)
    if (schema.defines("$ref")) {
      return false;
    }

    // The single allOf branch must contain $ref
    const auto &branch = schema.at("allOf").front();
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the single branch from allOf
    auto branch = schema.at("allOf").front();

    // Remove the allOf keyword
    schema.erase("allOf");

    // Merge all properties from the branch into the parent schema
    for (const auto &pair : branch.as_object()) {
      schema.assign(pair.first, pair.second);
    }
  }
};
