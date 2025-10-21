class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In modern JSON Schema (2019-09+), `$ref` no longer overrides "
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
    // Only apply to modern drafts (2019-09 and 2020-12)
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

    // allOf must be an array
    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.empty()) {
      return false;
    }

    // Parent must not already have a $ref (can't have two $refs)
    if (schema.defines("$ref")) {
      return false;
    }

    // Count how many branches contain a top-level $ref
    std::size_t ref_count = 0;
    std::size_t ref_only_index = 0;
    for (std::size_t i = 0; i < allof.size(); ++i) {
      const auto &branch = allof.at(i);
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
        // Check if this branch is ref-only (only has $ref, no other keys)
        if (branch.size() == 1) {
          ref_only_index = i;
        }
      }
    }

    // Only apply if there's exactly one ref-only branch
    return ref_count == 1 && allof.at(ref_only_index).size() == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto allof = schema.at("allOf");

    // Find the ref-only branch
    auto ref_it = allof.as_array().end();
    for (auto it = allof.as_array().begin(); it != allof.as_array().end();
         ++it) {
      if (it->is_object() && it->defines("$ref") && it->size() == 1) {
        ref_it = it;
        break;
      }
    }

    // Extract the $ref value
    auto ref_value = ref_it->at("$ref");

    // Remove the ref-only branch from allOf
    allof.erase(ref_it, ref_it + 1);

    // If allOf is now empty, remove it entirely
    if (allof.empty()) {
      schema.erase("allOf");
    } else {
      // Update the allOf in the schema
      schema.at("allOf").into(std::move(allof));
    }

    // Add the $ref to the parent schema
    schema.assign("$ref", std::move(ref_value));
  }
};
