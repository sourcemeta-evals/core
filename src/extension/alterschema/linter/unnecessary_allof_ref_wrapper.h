class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In modern JSON Schema versions (2019-09+), wrapping `$ref` in "
            "`allOf` is no longer necessary as `$ref` no longer overrides "
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
    // Only applies to 2019-09 and later
    ONLY_CONTINUE_IF(contains_any(
        vocabularies,
        {"https://json-schema.org/draft/2020-12/vocab/applicator",
         "https://json-schema.org/draft/2019-09/vocab/applicator"}));

    ONLY_CONTINUE_IF(schema.is_object() && schema.defines("allOf") &&
                     schema.at("allOf").is_array());

    // Don't apply if there's already a $ref sibling to allOf
    ONLY_CONTINUE_IF(!schema.defines("$ref"));

    const auto &all_of{schema.at("allOf")};

    // This rule specifically targets the case where allOf has a single
    // $ref-only branch. Other cases are handled by unnecessary_allof_wrapper_*.
    ONLY_CONTINUE_IF(all_of.size() == 1);

    const auto &entry{all_of.at(0)};
    ONLY_CONTINUE_IF(entry.is_object() && entry.size() == 1 &&
                     entry.defines("$ref"));

    return APPLIES_TO_POINTERS({Pointer{"allOf", 0}});
  }

  auto transform(JSON &schema, const Result &) const -> void override {
    // Get the $ref value from the single branch
    auto ref_value = schema.at("allOf").at(0).at("$ref");

    // Insert $ref BEFORE allOf (while it still exists) to preserve ordering
    schema.try_assign_before("$ref", std::move(ref_value), "allOf");

    // Now erase the $ref keyword from the branch (making it empty)
    schema.at("allOf").at(0).erase("$ref");

    // Remove empty branches
    schema.at("allOf").erase_if(sourcemeta::core::is_empty_schema);

    // If allOf is now empty, remove it
    if (schema.at("allOf").empty()) {
      schema.erase("allOf");
    }
  }
};
