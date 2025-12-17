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
    ONLY_CONTINUE_IF(contains_any(
        vocabularies,
        {"https://json-schema.org/draft/2020-12/vocab/applicator",
         "https://json-schema.org/draft/2019-09/vocab/applicator"}));
    ONLY_CONTINUE_IF(schema.is_object() && schema.defines("allOf") &&
                     schema.at("allOf").is_array());

    // Don't apply if there's already a $ref at the parent level
    ONLY_CONTINUE_IF(!schema.defines("$ref"));

    const auto &all_of = schema.at("allOf");

    // Only apply when allOf has exactly one branch that contains ONLY $ref
    // This is a specific case not handled by UnnecessaryAllOfWrapperModern
    // which extracts keywords but doesn't handle the case of a single $ref
    // branch that should completely replace the allOf
    ONLY_CONTINUE_IF(all_of.size() == 1);

    const auto &branch = all_of.at(0);
    ONLY_CONTINUE_IF(branch.is_object() && branch.defines("$ref") &&
                     branch.size() == 1);

    return APPLIES_TO_KEYWORDS("allOf");
  }

  auto transform(JSON &schema, const Result &) const -> void override {
    const auto &all_of = schema.at("allOf");
    const auto &branch = all_of.at(0);
    const auto ref_value = branch.at("$ref").to_string();

    // Use try_assign_before to maintain proper keyword ordering
    // (place $ref where allOf was in the keyword order)
    schema.try_assign_before("$ref", JSON{ref_value}, "allOf");
    schema.erase("allOf");
  }
};
