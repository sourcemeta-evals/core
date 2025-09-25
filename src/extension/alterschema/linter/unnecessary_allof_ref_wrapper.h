class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{"unnecessary_allof_ref_wrapper",
                            "Unnecessary `allOf` wrapper around `$ref` in "
                            "modern JSON Schema versions. "
                            "In 2019-09 and later, `$ref` no longer overrides "
                            "sibling keywords, "
                            "so wrapping it in `allOf` is redundant when there "
                            "are no other branches."} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    // Only apply to 2019-09 and later versions
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

    const auto &allof = schema.at("allOf");

    // allOf must be an array with exactly one element
    if (!allof.is_array() || allof.size() != 1) {
      return false;
    }

    // The single element must contain $ref
    const auto &branch = allof.at(0);
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    // There must be no $ref sibling to allOf (this would change semantics)
    if (schema.defines("$ref")) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the $ref from the single allOf branch
    auto ref_value = schema.at("allOf").at(0).at("$ref");

    // Remove the allOf
    schema.erase("allOf");

    // Add the $ref at the top level
    schema.assign("$ref", std::move(ref_value));
  }
};
