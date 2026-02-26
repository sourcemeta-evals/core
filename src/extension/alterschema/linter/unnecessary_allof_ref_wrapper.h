class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` no longer overrides "
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

    // Count how many branches contain $ref
    std::size_t ref_count{0};
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
      }
    }

    // Only apply if exactly one branch contains $ref
    // and that branch only has $ref (nothing else)
    if (ref_count != 1) {
      return false;
    }

    // Find the single branch with $ref and ensure it only contains $ref
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref") && branch.size() == 1) {
        return true;
      }
    }

    return false;
  }

  auto transform(JSON &schema) const -> void override {
    // Find the $ref value from the allOf branch
    JSON ref_value{nullptr};
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref") && branch.size() == 1) {
        ref_value = branch.at("$ref");
        break;
      }
    }

    // If allOf has only one branch, remove allOf entirely and set $ref
    if (schema.at("allOf").size() == 1) {
      schema.erase("allOf");
    } else {
      // Remove only the $ref branch from allOf
      for (auto it = schema.at("allOf").as_array().cbegin();
           it != schema.at("allOf").as_array().cend(); ++it) {
        if (it->is_object() && it->defines("$ref") && it->size() == 1) {
          schema.at("allOf").erase(it);
          break;
        }
      }
    }

    schema.assign("$ref", std::move(ref_value));
  }
};
