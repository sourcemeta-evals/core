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
    // Only applies to 2019-09 and later
    if (!contains_any(
            vocabularies,
            {"https://json-schema.org/draft/2020-12/vocab/applicator",
             "https://json-schema.org/draft/2019-09/vocab/applicator"})) {
      return false;
    }

    // Must have allOf
    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.size() != 1) {
      return false;
    }

    // The single branch must be an object with only $ref
    const auto &branch = allof.at(0);
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    // Check if there are other keywords at the same level as allOf
    // If there are, we can't extract the $ref because it would become a sibling
    // However, if the only other keywords are annotations like $schema, $id,
    // etc., we can still extract
    bool has_non_annotation_siblings = false;
    for (const auto &pair : schema.as_object()) {
      const auto &key = pair.first;
      if (key != "allOf" && key != "$schema" && key != "$id" &&
          key != "$anchor" && key != "$defs" && key != "definitions" &&
          key != "$comment" && key != "title" && key != "description" &&
          key != "default" && key != "examples" && key != "deprecated" &&
          key != "readOnly" && key != "writeOnly") {
        has_non_annotation_siblings = true;
        break;
      }
    }

    // If there are non-annotation siblings, we cannot extract the $ref
    // because it would become a sibling to those keywords
    if (has_non_annotation_siblings) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Extract the $ref from the allOf array
    auto ref_value = schema.at("allOf").at(0).at("$ref");
    schema.erase("allOf");
    schema.assign("$ref", std::move(ref_value));
  }
};
