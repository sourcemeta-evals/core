class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In modern JSON Schema versions (2019-09 and later), `$ref` no "
            "longer overrides sibling keywords, so wrapping a `$ref` in "
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
    if (!contains_any(
            vocabularies,
            {"https://json-schema.org/draft/2020-12/vocab/applicator",
             "https://json-schema.org/draft/2019-09/vocab/applicator"})) {
      return false;
    }

    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array() || schema.at("allOf").empty()) {
      return false;
    }

    // Don't apply if there is already a $ref sibling to allOf
    if (schema.defines("$ref")) {
      return false;
    }

    // Count how many branches in allOf contain $ref
    std::size_t ref_count = 0;
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
      }
    }

    // Don't apply if more than one branch contains $ref
    if (ref_count != 1) {
      return false;
    }

    // Check that the branch with $ref has ONLY $ref (no other keywords)
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref") &&
          branch.object_size() == 1) {
        return true;
      }
    }

    return false;
  }

  auto transform(JSON &schema) const -> void override {
    // Find the branch with $ref and extract it
    const auto &allof = schema.at("allOf");
    JSON ref_value{nullptr};
    std::size_t ref_index = 0;

    for (std::size_t index = 0; index < allof.size(); index++) {
      const auto &branch = allof.at(index);
      if (branch.is_object() && branch.defines("$ref") &&
          branch.object_size() == 1) {
        ref_value = branch.at("$ref");
        ref_index = index;
        break;
      }
    }

    // Remove the $ref branch from allOf
    if (allof.size() == 1) {
      // If allOf has only one element, remove allOf entirely
      schema.erase("allOf");
    } else {
      // Remove the specific branch from allOf
      auto it = schema.at("allOf").as_array().cbegin();
      std::advance(it, static_cast<std::ptrdiff_t>(ref_index));
      schema.at("allOf").erase(it);
    }

    // Assign the $ref to the schema
    schema.assign("$ref", std::move(ref_value));
  }
};
