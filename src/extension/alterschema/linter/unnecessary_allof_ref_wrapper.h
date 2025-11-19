class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In modern JSON Schema versions (2019-09 and later), `$ref` no "
            "longer overrides sibling keywords, so wrapping a single `$ref` "
            "in `allOf` is unnecessary"} {};

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

    // allOf must be an array
    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.empty()) {
      return false;
    }

    // Count how many branches contain $ref
    std::size_t ref_count = 0;
    for (const auto &branch : allof.as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
      }
    }

    // Only apply if there's exactly one branch with $ref
    if (ref_count != 1) {
      return false;
    }

    // Only apply if allOf has exactly one branch total
    // (otherwise we'd be removing other branches)
    if (allof.size() != 1) {
      return false;
    }

    // Only apply if there are no sibling keywords to allOf (except $schema)
    // If there are siblings, we can't just extract the $ref
    for (const auto &entry : schema.as_object()) {
      if (entry.first != "allOf" && entry.first != "$schema") {
        return false;
      }
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the single branch from allOf
    auto branch = schema.at("allOf").at(0);

    // Remove the allOf keyword
    schema.erase("allOf");

    // Merge the branch into the schema
    if (branch.is_object()) {
      for (auto &entry : branch.as_object()) {
        schema.assign(entry.first, std::move(entry.second));
      }
    }
  }
};
