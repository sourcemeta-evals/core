class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` no longer overrides "
            "sibling keywords, making `allOf` wrappers around single `$ref` "
            "instances unnecessary"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    // Only apply to 2019-09 and 2020-12
    if (!contains_any(
            vocabularies,
            {"https://json-schema.org/draft/2020-12/vocab/applicator",
             "https://json-schema.org/draft/2019-09/vocab/applicator"})) {
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

    // Check if there's only one branch with a single $ref
    if (allof.size() == 1) {
      const auto &branch = allof.front();
      // The single branch must be an object with only $ref
      return branch.is_object() && branch.defines("$ref") && branch.size() == 1;
    }

    // If there are multiple branches, check if only one contains $ref
    // and that branch only contains $ref
    std::size_t ref_branch_count = 0;
    for (const auto &branch : allof.as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_branch_count++;
      }
    }

    // Only apply if there's exactly one branch with $ref and that branch
    // contains only $ref
    if (ref_branch_count == 1) {
      for (const auto &branch : allof.as_array()) {
        if (branch.is_object() && branch.defines("$ref") &&
            branch.size() == 1) {
          return true;
        }
      }
    }

    return false;
  }

  auto transform(JSON &schema) const -> void override {
    auto allof = schema.at("allOf");

    // Case 1: Single branch with only $ref
    if (allof.size() == 1) {
      const auto &branch = allof.front();
      // Copy the $ref value
      auto ref_value = branch.at("$ref");
      // Remove allOf
      schema.erase("allOf");
      // Add $ref to parent
      schema.assign("$ref", std::move(ref_value));
      return;
    }

    // Case 2: Multiple branches, extract the one with only $ref
    JSON ref_value{nullptr};
    auto ref_position = allof.as_array().end();
    for (auto it = allof.as_array().begin(); it != allof.as_array().end();
         ++it) {
      if (it->is_object() && it->defines("$ref") && it->size() == 1) {
        ref_value = it->at("$ref");
        ref_position = it;
        break;
      }
    }

    // Remove the $ref branch from allOf
    allof.erase(ref_position);

    // If only one branch remains, unwrap it
    if (allof.size() == 1) {
      auto remaining = allof.front();
      schema.erase("allOf");
      // Merge the remaining branch into the parent
      if (remaining.is_object()) {
        for (const auto &entry : remaining.as_object()) {
          schema.assign(entry.first, entry.second);
        }
      }
    } else {
      // More than one branch remains, update allOf
      schema.at("allOf").into(std::move(allof));
    }

    // Add $ref to parent
    schema.assign("$ref", std::move(ref_value));
  }
};
