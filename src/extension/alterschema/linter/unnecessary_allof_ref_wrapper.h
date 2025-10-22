class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In modern JSON Schema versions (2019-09 and later), wrapping a "
            "single `$ref` in `allOf` is unnecessary as `$ref` no longer "
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
    // Only apply to modern JSON Schema versions (2019-09 and 2020-12)
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

    // Schema must not already have a $ref at the same level
    if (schema.defines("$ref")) {
      return false;
    }

    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.empty()) {
      return false;
    }

    // Count branches that are objects with exactly one key: "$ref"
    std::size_t ref_only_count = 0;
    for (const auto &branch : allof.as_array()) {
      if (branch.is_object() && branch.size() == 1 && branch.defines("$ref")) {
        ref_only_count++;
      }
    }

    // Only apply if there is exactly one $ref-only branch
    return ref_only_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto allof_copy = schema.at("allOf");

    // Find the $ref-only branch and extract its value
    auto &allof_array = allof_copy.as_array();
    auto it = allof_array.begin();
    bool found = false;

    for (; it != allof_array.end(); ++it) {
      if (it->is_object() && it->size() == 1 && it->defines("$ref")) {
        // Add the $ref to the parent schema
        schema.assign("$ref", it->at("$ref"));
        // Remove this branch from allOf
        allof_copy.erase(it);
        found = true;
        break;
      }
    }

    if (!found) {
      return;
    }

    // If allOf is now empty, remove it entirely
    if (allof_copy.empty()) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(allof_copy));
    }
  }
};
