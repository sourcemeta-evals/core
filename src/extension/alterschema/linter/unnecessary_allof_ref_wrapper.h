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
    ONLY_CONTINUE_IF(contains_any(
        vocabularies,
        {"https://json-schema.org/draft/2020-12/vocab/applicator",
         "https://json-schema.org/draft/2019-09/vocab/applicator"}));
    ONLY_CONTINUE_IF(schema.is_object() && schema.defines("allOf") &&
                     schema.at("allOf").is_array());
    ONLY_CONTINUE_IF(!schema.defines("$ref"));

    const auto &all_of = schema.at("allOf");

    // Only apply to single-element allOf arrays to avoid interfering with
    // UnnecessaryAllOfWrapperModern which handles multi-element arrays
    ONLY_CONTINUE_IF(all_of.size() == 1);

    const auto &branch = all_of.at(0);
    // Only apply when the single element contains only $ref
    ONLY_CONTINUE_IF(branch.is_object() && branch.defines("$ref") &&
                     branch.size() == 1);

    return APPLIES_TO_KEYWORDS(Pointer{"allOf", 0});
  }

  auto transform(JSON &schema, const Result &) const -> void override {
    // Extract $ref from the single-element allOf, preserving keyword ordering
    const auto ref_value = schema.at("allOf").at(0).at("$ref");
    schema.try_assign_before("$ref", ref_value, "allOf");
    schema.erase("allOf");
  }
};
