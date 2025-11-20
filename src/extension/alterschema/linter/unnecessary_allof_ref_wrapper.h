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

    const auto &branch = allof_array.front();

    // The single branch must be an object with only $ref
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    // The branch should only contain $ref (no sibling keywords)
    // If there are other keywords, we can't safely extract
    if (branch.size() != 1) {
      return false;
    }

    // Also check that the parent schema doesn't already have a $ref
    // If it does, we can't extract (would create a conflict)
    if (schema.defines("$ref")) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the $ref from the allOf branch
    auto ref_value = schema.at("allOf").front().at("$ref");

    // Remove the allOf keyword
    schema.erase("allOf");

    // Add the $ref at the parent level
    schema.assign("$ref", std::move(ref_value));
  }
};
