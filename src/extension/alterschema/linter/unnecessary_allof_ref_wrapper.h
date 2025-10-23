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

    // Must have exactly one branch
    if (allof_array.size() != 1) {
      return false;
    }

    // That branch must be an object with $ref
    const auto &branch = allof_array.at(0);
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    // Must not have $ref as a sibling to allOf
    // (if there's a sibling $ref, the allOf is meaningful)
    if (schema.defines("$ref")) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the single branch from allOf
    auto branch = schema.at("allOf").at(0);

    // Remove the allOf keyword
    schema.erase("allOf");

    // Move all properties from the branch to the parent schema
    for (auto &entry : branch.as_object()) {
      schema.assign(entry.first, std::move(entry.second));
    }
  }
};
