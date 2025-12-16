class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` can have sibling "
            "keywords, so wrapping a single `$ref` in `allOf` is unnecessary"} {
        };

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

    // Cannot have $ref sibling to allOf
    if (schema.defines("$ref")) {
      return false;
    }

    const auto &all_of = schema.at("allOf");

    // Count branches that contain $ref
    std::size_t ref_branch_count = 0;
    std::size_t ref_branch_index = 0;

    for (std::size_t index = 0; index < all_of.size(); ++index) {
      const auto &branch = all_of.at(index);
      if (branch.is_object() && branch.defines("$ref")) {
        ref_branch_count++;
        ref_branch_index = index;
      }
    }

    // Must have exactly one branch with $ref
    if (ref_branch_count != 1) {
      return false;
    }

    // The branch with $ref must only contain $ref (no other keywords)
    const auto &ref_branch = all_of.at(ref_branch_index);
    return ref_branch.size() == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto collection = schema.at("allOf");

    // Find the branch with $ref
    std::size_t ref_branch_index = 0;
    for (std::size_t index = 0; index < collection.size(); ++index) {
      const auto &branch = collection.at(index);
      if (branch.is_object() && branch.defines("$ref")) {
        ref_branch_index = index;
        break;
      }
    }

    // Extract the $ref value
    auto ref_value = collection.at(ref_branch_index).at("$ref");

    // Remove the branch from allOf
    collection.erase(collection.as_array().begin() +
                     static_cast<std::ptrdiff_t>(ref_branch_index));

    // If allOf is now empty, remove it entirely
    if (collection.empty()) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(collection));
    }

    // Add $ref to the schema
    schema.assign("$ref", std::move(ref_value));
  }
};
