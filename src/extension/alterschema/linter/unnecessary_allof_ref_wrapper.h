class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` no longer overrides "
            "sibling keywords, so wrapping a `$ref` in `allOf` is "
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
    // Only apply to 2019-09 and 2020-12
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

    // Must not have $ref sibling
    if (schema.defines("$ref")) {
      return false;
    }

    // Count branches that contain only $ref
    const auto &all_of = schema.at("allOf");
    std::size_t ref_only_count = 0;

    for (const auto &branch : all_of.as_array()) {
      if (branch.is_object() && branch.size() == 1 && branch.defines("$ref")) {
        ref_only_count++;
      }
    }

    // Apply only if exactly one branch contains only $ref
    return ref_only_count == 1;
  }

  auto transform(JSON &schema, const Result &) const -> void override {
    auto collection = schema.at("allOf");

    // Find and extract the branch with only $ref
    for (std::size_t i = 0; i < collection.size(); ++i) {
      const auto &branch = collection.at(i);
      if (branch.is_object() && branch.size() == 1 && branch.defines("$ref")) {
        // Copy the $ref to the parent schema, placing it before allOf
        schema.try_assign_before("$ref", branch.at("$ref"), "allOf");

        // Remove this branch from allOf
        collection.erase(collection.as_array().begin() +
                         static_cast<std::ptrdiff_t>(i));
        break;
      }
    }

    // If allOf is now empty, remove it; otherwise update it
    if (collection.empty()) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(collection));
    }
  }
};
