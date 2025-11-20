class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Since Draft 2019-09, wrapping a single `$ref` in `allOf` is "
            "unnecessary as `$ref` no longer overrides sibling keywords"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    // Only applies to 2019-09 and later (where $ref doesn't override siblings)
    if (!contains_any(vocabularies,
                      {"https://json-schema.org/draft/2020-12/vocab/core",
                       "https://json-schema.org/draft/2019-09/vocab/core"})) {
      return false;
    }

    // Must be an object with allOf
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
    std::size_t ref_index = 0;

    for (std::size_t i = 0; i < allof.size(); i++) {
      const auto &branch = allof.at(i);
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
        ref_index = i;
      }
    }

    // Only apply if there's exactly one branch with $ref
    if (ref_count != 1) {
      return false;
    }

    // Check if the schema has no sibling to allOf (except $schema, $id, etc.)
    // If there are siblings to allOf, we can't extract the $ref
    // because it would change the semantics
    for (const auto &entry : schema.as_object()) {
      const auto &key = entry.first;
      // Skip allOf itself and metadata keywords that don't affect validation
      if (key != "allOf" && key != "$schema" && key != "$id" &&
          key != "$comment" && key != "title" && key != "description" &&
          key != "examples" && key != "default" && key != "$defs" &&
          key != "definitions") {
        return false;
      }
    }

    // We can only extract if the $ref branch has no other keywords
    // (i.e., it only contains $ref and nothing else)
    const auto &ref_branch = allof.at(ref_index);
    if (ref_branch.size() != 1) {
      return false;
    }

    // If allOf has only one branch total, we can extract it
    if (allof.size() == 1) {
      return true;
    }

    // If allOf has multiple branches, we can extract the $ref
    // since it has no siblings in its branch
    return true;
  }

  auto transform(JSON &schema) const -> void override {
    auto allof = schema.at("allOf");

    // Find the branch with $ref
    std::size_t ref_index = 0;
    for (std::size_t i = 0; i < allof.size(); i++) {
      const auto &branch = allof.at(i);
      if (branch.is_object() && branch.defines("$ref")) {
        ref_index = i;
        break;
      }
    }

    const auto &ref_branch = allof.at(ref_index);

    if (allof.size() == 1) {
      // If there's only one branch, replace allOf with its content
      auto content = ref_branch;
      schema.erase("allOf");
      for (const auto &entry : content.as_object()) {
        schema.assign(entry.first, entry.second);
      }
    } else {
      // If there are multiple branches, just remove the $ref branch
      // and keep the rest in allOf
      auto new_allof = JSON::make_array();
      for (std::size_t i = 0; i < allof.size(); i++) {
        if (i != ref_index) {
          new_allof.push_back(allof.at(i));
        }
      }

      // Extract the $ref to the parent schema
      schema.assign("$ref", ref_branch.at("$ref"));

      // Update or remove allOf
      if (new_allof.size() == 1) {
        // If only one branch remains, merge it into the parent
        schema.erase("allOf");
        const auto &remaining = new_allof.at(0);
        if (remaining.is_object()) {
          for (const auto &entry : remaining.as_object()) {
            if (!schema.defines(entry.first)) {
              schema.assign(entry.first, entry.second);
            }
          }
        }
      } else {
        // Keep allOf with remaining branches
        schema.at("allOf").into(std::move(new_allof));
      }
    }
  }
};
