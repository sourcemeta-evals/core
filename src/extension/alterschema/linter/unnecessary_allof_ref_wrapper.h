class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a `$ref` in `allOf` is unnecessary in modern JSON Schema "
            "versions (2019-09 and later) where `$ref` no longer overrides "
            "sibling keywords"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    // Only apply to 2019-09 and later
    if (!contains_any(vocabularies,
                      {"https://json-schema.org/draft/2020-12/vocab/core",
                       "https://json-schema.org/draft/2019-09/vocab/core"})) {
      return false;
    }

    // Must be an object with allOf
    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    // Skip if there's already a $ref sibling to allOf
    if (schema.defines("$ref")) {
      return false;
    }

    // allOf must be an array
    const auto &allof = schema.at("allOf");
    if (!allof.is_array()) {
      return false;
    }

    // Count branches that contain $ref
    std::size_t ref_count = 0;
    for (const auto &branch : allof.as_array()) {
      if (branch.is_object() && branch.defines("$ref") &&
          branch.at("$ref").is_string()) {
        // Skip if branch contains $id (resource boundary)
        if (branch.defines("$id") || branch.defines("id")) {
          return false;
        }
        // Skip if branch contains dynamic refs
        if (branch.defines("$dynamicRef") || branch.defines("$recursiveRef")) {
          return false;
        }
        ref_count++;
      }
    }

    // Only apply if exactly one branch contains $ref
    return ref_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto allof = schema.at("allOf");

    // Find the branch with $ref and extract the $ref value
    auto ref_value = allof.at(0); // Initialize with first element
    for (auto &branch : allof.as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_value = branch.at("$ref");
        branch.erase("$ref");
        break;
      }
    }

    // Remove empty branches from allOf
    auto last = std::remove_if(
        allof.as_array().begin(), allof.as_array().end(),
        [](const auto &item) { return item.is_object() && item.empty(); });
    allof.erase(last, allof.as_array().end());

    // Update or remove allOf
    if (allof.empty()) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(allof));
    }

    // Add $ref to the schema
    schema.assign("$ref", ref_value);
  }
};
