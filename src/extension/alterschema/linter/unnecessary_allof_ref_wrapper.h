class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a single `$ref` in `allOf` is unnecessary in modern JSON "
            "Schema versions (2019-09+), as `$ref` no longer overrides sibling "
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

    const auto &single_element = allof.at(0);

    if (!single_element.is_object() || !single_element.defines("$ref") ||
        single_element.size() != 1) {
      return false;
    }

    // Ensure the $ref value is actually a string
    if (!single_element.at("$ref").is_string()) {
      return false;
    }

    // Check if there are any sibling keywords to allOf (except pure meta
    // keywords)
    for (const auto &entry : schema.as_object()) {
      const auto &key = entry.first;
      if (key != "allOf" && key != "$schema" && key != "$id" &&
          key != "$anchor" && key != "$defs" && key != "definitions" &&
          key != "$comment") {
        return false;
      }
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Make a copy of the $ref value before erasing allOf
    const auto ref_value = schema.at("allOf").at(0).at("$ref");

    schema.erase("allOf");

    schema.assign("$ref", ref_value);
  }
};
