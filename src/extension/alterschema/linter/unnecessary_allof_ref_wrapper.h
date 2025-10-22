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
    // Only apply to modern drafts (2019-09 and 2020-12)
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
    if (!schema.at("allOf").is_array()) {
      return false;
    }

    // Must not have a $ref sibling to allOf
    if (schema.defines("$ref")) {
      return false;
    }

    const auto &allof_array = schema.at("allOf");

    // allOf must have at least one element
    if (allof_array.empty()) {
      return false;
    }

    // Count branches that contain $ref
    std::size_t ref_branch_count = 0;
    std::size_t ref_branch_index = 0;

    for (std::size_t i = 0; i < allof_array.size(); ++i) {
      const auto &branch = allof_array.at(i);
      if (branch.is_object() && branch.defines("$ref")) {
        ref_branch_count++;
        ref_branch_index = i;
      }
    }

    // Only apply if exactly one branch contains $ref
    if (ref_branch_count != 1) {
      return false;
    }

    // Check for key collisions between the $ref branch and other branches
    using String = JSON::String;
    const auto &ref_branch = allof_array.at(ref_branch_index);
    std::vector<String> ref_branch_keys;
    for (const auto &entry : ref_branch.as_object()) {
      ref_branch_keys.push_back(entry.first);
    }

    // Collect all keys from non-$ref branches
    std::vector<String> other_keys;
    for (std::size_t i = 0; i < allof_array.size(); ++i) {
      if (i == ref_branch_index) {
        continue;
      }

      const auto &branch = allof_array.at(i);
      // Only process object branches
      if (!branch.is_object()) {
        // Skip transformation if there are non-object branches
        return false;
      }

      for (const auto &entry : branch.as_object()) {
        other_keys.push_back(entry.first);
      }
    }

    // Check for key collisions between ref_branch and other branches
    for (const auto &key : ref_branch_keys) {
      if (std::find(other_keys.begin(), other_keys.end(), key) !=
          other_keys.end()) {
        return false;
      }
    }

    // Also check for collisions with parent schema keys (excluding allOf)
    for (const auto &entry : schema.as_object()) {
      const String key{entry.first};
      if (key == "allOf") {
        continue;
      }

      // Check collision with ref_branch
      if (std::find(ref_branch_keys.begin(), ref_branch_keys.end(), key) !=
          ref_branch_keys.end()) {
        return false;
      }

      // Check collision with other branches
      if (std::find(other_keys.begin(), other_keys.end(), key) !=
          other_keys.end()) {
        return false;
      }
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    using String = JSON::String;
    const auto &allof_array = schema.at("allOf");

    // Find the branch with $ref
    std::size_t ref_branch_index = 0;
    for (std::size_t i = 0; i < allof_array.size(); ++i) {
      const auto &branch = allof_array.at(i);
      if (branch.is_object() && branch.defines("$ref")) {
        ref_branch_index = i;
        break;
      }
    }

    // Stage all assignments before mutating schema
    std::vector<std::pair<String, JSON>> staged;

    // Collect all properties from the $ref branch
    const auto &ref_branch = allof_array.at(ref_branch_index);
    for (const auto &kv : ref_branch.as_object()) {
      const String key{kv.first};
      staged.emplace_back(key, kv.second);
    }

    // Collect all properties from other branches
    for (std::size_t i = 0; i < allof_array.size(); ++i) {
      if (i == ref_branch_index) {
        continue;
      }

      const auto &branch = allof_array.at(i);
      for (const auto &kv : branch.as_object()) {
        const String key{kv.first};
        staged.emplace_back(key, kv.second);
      }
    }

    // Remove the allOf keyword first
    schema.erase("allOf");

    // Now assign all staged properties to the parent
    for (auto &kv : staged) {
      schema.assign(kv.first, std::move(kv.second));
    }
  }
};
