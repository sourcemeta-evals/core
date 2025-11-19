class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In modern JSON Schema versions (2019-09 and later), `$ref` no "
            "longer overrides sibling keywords, so wrapping a single `$ref` in "
            "`allOf` is unnecessary"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    // Only applies to 2019-09 and 2020-12
    if (!contains_any(
            vocabularies,
            {"https://json-schema.org/draft/2020-12/vocab/applicator",
             "https://json-schema.org/draft/2019-09/vocab/applicator"})) {
      return false;
    }

    // Schema must be an object with allOf
    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array()) {
      return false;
    }

    // Cannot extract if there's already a $ref sibling
    if (schema.defines("$ref")) {
      return false;
    }

    const auto &all_of = schema.at("allOf");

    // Count how many branches contain $ref
    std::size_t ref_count = 0;
    for (const auto &branch : all_of.as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
      }
    }

    // Should not apply if more than one branch contains $ref
    if (ref_count != 1) {
      return false;
    }

    // Find the branch with $ref and check if it only contains $ref
    for (const auto &branch : all_of.as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        // The branch with $ref should only contain $ref (size == 1)
        if (branch.size() == 1) {
          return true;
        }
        break;
      }
    }

    return false;
  }

  auto transform(JSON &schema) const -> void override {
    auto all_of = schema.at("allOf");

    // Find the branch with $ref
    std::size_t ref_index = 0;
    for (std::size_t i = 0; i < all_of.size(); i++) {
      if (all_of.at(i).is_object() && all_of.at(i).defines("$ref")) {
        ref_index = i;
        break;
      }
    }

    // Extract the $ref value
    auto ref_value = all_of.at(ref_index).at("$ref");

    if (all_of.size() == 1) {
      // Single element case: remove allOf entirely and add $ref
      schema.erase("allOf");
      schema.assign("$ref", std::move(ref_value));
    } else {
      // Multiple elements case: extract $ref and remove that branch from allOf
      schema.assign("$ref", std::move(ref_value));
      all_of.erase(all_of.as_array().begin() +
                   static_cast<std::ptrdiff_t>(ref_index));
      schema.at("allOf").into(std::move(all_of));
    }
  }
};
