class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a single `$ref` in `allOf` is unnecessary in modern "
            "JSON Schema versions (2019-09 and later) where `$ref` no longer "
            "overrides sibling keywords"} {};

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

    const auto &branch = allof_array.at(0);

    // Branch must be an object with only $ref
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    // Branch must have only $ref (no other keywords)
    if (branch.size() != 1) {
      return false;
    }

    // Parent schema must not have $ref as a sibling to allOf
    // (otherwise we'd create a conflict)
    if (schema.defines("$ref")) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the $ref from the allOf branch
    auto ref_value = schema.at("allOf").at(0).at("$ref");

    // Remove the allOf
    schema.erase("allOf");

    // Add the $ref to the parent schema
    schema.assign("$ref", std::move(ref_value));
  }
};
