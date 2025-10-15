class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In modern JSON Schema (2019-09 and later), `$ref` no longer "
            "overrides sibling keywords, so wrapping a single `$ref` in "
            "`allOf` is unnecessary"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    // Only applies to 2019-09 and later where $ref doesn't override siblings
    if (!contains_any(
            vocabularies,
            {"https://json-schema.org/draft/2020-12/vocab/applicator",
             "https://json-schema.org/draft/2019-09/vocab/applicator"})) {
      return false;
    }

    // Must have allOf and it must be an array
    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array()) {
      return false;
    }

    const auto &allof_array = schema.at("allOf");

    // Must have exactly one item in allOf
    if (allof_array.size() != 1) {
      return false;
    }

    // That single item must be an object with $ref
    const auto &item = allof_array.front();
    if (!item.is_object() || !item.defines("$ref")) {
      return false;
    }

    // The schema must not already have a $ref at the same level as allOf
    // (to avoid creating conflicting $refs)
    if (schema.defines("$ref")) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Get the single item from allOf
    auto item = schema.at("allOf").front();

    // Remove the allOf keyword
    schema.erase("allOf");

    // Merge all properties from the item into the parent schema
    // This will include the $ref and any other properties
    for (const auto &entry : item.as_object()) {
      schema.assign(entry.first, entry.second);
    }
  }
};
