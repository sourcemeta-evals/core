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
    // Only applies to 2019-09 and later
    if (!contains_any(vocabularies,
                      {"https://json-schema.org/draft/2020-12/vocab/core",
                       "https://json-schema.org/draft/2019-09/vocab/core"})) {
      return false;
    }

    // Must be an object with allOf
    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    // allOf must be an array
    const auto &allof = schema.at("allOf");
    if (!allof.is_array()) {
      return false;
    }

    // allOf must have exactly one branch
    if (allof.size() != 1) {
      return false;
    }

    // The single branch must be an object with only $ref
    const auto &branch = allof.front();
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    // The branch must contain only $ref (no other keywords)
    if (branch.size() != 1) {
      return false;
    }

    // The schema must not have $ref as a sibling to allOf
    // (otherwise we'd be creating a duplicate $ref)
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

    // Add the $ref at the schema level
    schema.assign("$ref", std::move(ref_value));
  }
};
