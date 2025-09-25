class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{"unnecessary_allof_ref_wrapper",
                            "Unnecessary `allOf` wrapper around `$ref` in "
                            "modern JSON Schema versions"} {};

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

    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array() || schema.at("allOf").size() != 1) {
      return false;
    }

    const auto &allof_item = schema.at("allOf").at(0);
    if (!allof_item.is_object() || !allof_item.defines("$ref")) {
      return false;
    }

    // Check if there are no sibling keywords to allOf (except $schema)
    for (const auto &entry : schema.as_object()) {
      const auto &key = entry.first;
      if (key != "allOf" && key != "$schema") {
        return false;
      }
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    const auto ref_value = schema.at("allOf").at(0).at("$ref");
    schema.erase("allOf");
    schema.assign("$ref", ref_value);
  }
};
