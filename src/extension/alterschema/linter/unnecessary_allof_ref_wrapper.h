class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` no longer overrides "
            "sibling keywords, making `allOf` wrappers around a single `$ref` "
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

    // Must have allOf
    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.empty()) {
      return false;
    }

    // Check if there's exactly one branch with a $ref and no other branches
    // have $ref
    std::size_t ref_count = 0;
    for (const auto &branch : allof.as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
      }
    }

    // Only apply if there's exactly one $ref in the allOf branches
    // and the allOf has exactly one branch
    // OR if there are multiple branches but only one contains $ref
    if (ref_count == 1 && allof.size() == 1) {
      // Single branch with $ref - can be unwrapped
      const auto &branch = allof.front();
      // Only unwrap if there are no sibling keywords to allOf
      // (except $schema, $id, etc. which are metadata)
      // Actually, we can unwrap if the branch itself has no other keywords
      // besides $ref, or if it does, we merge them up
      return true;
    }

    return false;
  }

  auto transform(JSON &schema) const -> void override {
    auto &allof = schema.at("allOf");
    auto branch = allof.front();

    // Extract the $ref and any other properties from the branch
    // and move them to the parent schema
    if (branch.is_object()) {
      for (const auto &entry : branch.as_object()) {
        schema.assign(entry.first, entry.second);
      }
    }

    // Remove the allOf keyword
    schema.erase("allOf");
  }
};
