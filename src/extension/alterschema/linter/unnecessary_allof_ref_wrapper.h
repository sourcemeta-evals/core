class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` no longer overrides "
            "sibling keywords, so wrapping a single `$ref` in `allOf` is "
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
    // Only applies to 2019-09 and later
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

    const auto &allof_array = schema.at("allOf");

    // allOf must have exactly one branch
    if (allof_array.size() != 1) {
      return false;
    }

    // The single branch must be an object with $ref
    const auto &branch = allof_array.front();
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    // The branch must only contain $ref (no sibling keywords)
    if (branch.size() != 1) {
      return false;
    }

    // The schema must not have $ref as a sibling to allOf
    if (schema.defines("$ref")) {
      return false;
    }

    // Only apply if the schema has no other keywords besides allOf and $schema
    // This prevents conflicts with UnnecessaryAllOfWrapperModern which handles
    // the case where there are other keywords in the parent schema
    for (const auto &entry : schema.as_object()) {
      if (entry.first != "allOf" && entry.first != "$schema") {
        return false;
      }
    }

    return true;
  }

  auto transform(JSON &schema, const Result &) const -> void override {
    // Extract the $ref from the single allOf branch
    auto ref_value = schema.at("allOf").front().at("$ref");

    // Remove the allOf keyword
    schema.erase("allOf");

    // Add the $ref at the top level
    schema.assign("$ref", std::move(ref_value));
  }
};
