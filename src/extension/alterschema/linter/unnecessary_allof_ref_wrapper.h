class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "An `allOf` with a single branch containing only a `$ref` can be "
            "simplified by extracting the `$ref` in modern JSON Schema "
            "versions "
            "(2019-09 and later)"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    // Only applies to 2019-09 and 2020-12 where $ref doesn't override siblings
    if (!contains_any(
            vocabularies,
            {"https://json-schema.org/draft/2020-12/vocab/applicator",
             "https://json-schema.org/draft/2019-09/vocab/applicator"})) {
      return false;
    }

    // Schema must be an object with allOf
    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.empty()) {
      return false;
    }

    // Count how many branches contain $ref
    std::size_t ref_count = 0;
    std::size_t single_ref_branch_index = 0;

    for (std::size_t i = 0; i < allof.size(); i++) {
      const auto &branch = allof.at(i);
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
        // If this branch has only $ref and nothing else, remember it
        if (branch.size() == 1) {
          single_ref_branch_index = i;
        }
      }
    }

    // Only apply if there's exactly one branch with a single $ref
    // (no other properties in that branch) and allOf has only one branch total
    return allof.size() == 1 && ref_count == 1 && allof.at(0).is_object() &&
           allof.at(0).size() == 1 && allof.at(0).defines("$ref") &&
           !schema.defines("$ref");
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the $ref from the single allOf branch
    auto ref_value = schema.at("allOf").at(0).at("$ref");
    schema.erase("allOf");
    schema.assign("$ref", std::move(ref_value));
  }
};
