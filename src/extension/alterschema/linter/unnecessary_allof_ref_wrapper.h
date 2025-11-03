class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a `$ref` in `allOf` is unnecessary in modern JSON "
            "Schema versions where `$ref` no longer overrides sibling "
            "keywords"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    if (!contains_any(
            vocabularies,
            {"https://json-schema.org/draft/2020-12/vocab/applicator",
             "https://json-schema.org/draft/2019-09/vocab/applicator"})) {
      return false;
    }

    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.size() != 1) {
      return false;
    }

    const auto &branch = allof.front();
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    // Check if there are no sibling keywords to allOf (except $schema, $id,
    // etc.) If there are other keywords at the same level as allOf, we
    // shouldn't unwrap
    if (schema.size() > 1) {
      for (const auto &entry : schema.as_object()) {
        const auto &key = entry.first;
        // Allow metadata keywords that don't affect validation
        if (key != "allOf" && key != "$schema" && key != "$id" &&
            key != "$comment" && key != "title" && key != "description" &&
            key != "default" && key != "examples" && key != "$defs" &&
            key != "definitions") {
          return false;
        }
      }
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    auto allof = schema.at("allOf");
    auto ref_schema = allof.front();

    // Move all properties from the ref schema to the parent
    for (auto &entry : ref_schema.as_object()) {
      schema.assign(entry.first, std::move(entry.second));
    }

    // Remove the allOf keyword
    schema.erase("allOf");
  }
};
