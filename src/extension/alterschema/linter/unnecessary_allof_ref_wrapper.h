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
    // Only apply to Draft 2019-09 and later (modern drafts where $ref doesn't
    // override siblings)
    if (!contains_any(
            vocabularies,
            {"https://json-schema.org/draft/2020-12/vocab/applicator",
             "https://json-schema.org/draft/2019-09/vocab/applicator"})) {
      return false;
    }

    // Must be an object with allOf array
    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array()) {
      return false;
    }

    // Parent must not already have a $ref (precondition: no $ref sibling to
    // allOf)
    if (schema.defines("$ref")) {
      return false;
    }

    const auto &allof_array = schema.at("allOf");

    // Count how many allOf branches have a top-level $ref
    std::size_t ref_count = 0;

    for (std::size_t i = 0; i < allof_array.size(); ++i) {
      const auto &branch = allof_array.at(i);
      if (branch.is_object() && branch.defines("$ref")) {
        // Only count if the branch is a "pure" ref (only has $ref, no other
        // keys) This avoids hoisting branches that have annotations or other
        // keywords
        if (branch.size() == 1) {
          ref_count++;
        }
      }
    }

    // Only apply if there's exactly one pure $ref branch
    return ref_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto collection = schema.at("allOf");

    // Find the pure $ref branch
    auto ref_iterator = collection.as_array().end();
    for (auto it = collection.as_array().begin();
         it != collection.as_array().end(); ++it) {
      if (it->is_object() && it->defines("$ref") && it->size() == 1) {
        ref_iterator = it;
        break;
      }
    }

    // Extract the $ref value and set it on the parent
    auto ref_value = ref_iterator->at("$ref");
    schema.assign("$ref", ref_value);

    // Remove the ref branch from allOf
    collection.erase(ref_iterator, std::next(ref_iterator));

    // If allOf is now empty, remove it; otherwise, write back the collection
    if (collection.size() == 0) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(collection));
    }
  }
};
