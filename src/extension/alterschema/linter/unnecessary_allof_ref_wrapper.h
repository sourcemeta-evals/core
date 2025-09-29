class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{"unnecessary_allof_ref_wrapper",
                            "Unnecessary `allOf` wrapper around `$ref` in "
                            "modern JSON Schema versions"} {}

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
    if (!branch.is_object() || !branch.defines("$ref") || branch.size() != 1) {
      return false;
    }

    for (const auto &pair : schema.as_object()) {
      const auto &key = pair.first;
      if (key != "allOf" && key != "$schema" && key != "$id" &&
          key != "$anchor" && key != "$dynamicAnchor" && key != "$ref" &&
          key != "$dynamicRef" && key != "$recursiveRef" &&
          key != "$recursiveAnchor") {
        return false;
      }
    }

    return true;
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    const auto ref_value = schema.at("allOf").front().at("$ref");
    schema.erase("allOf");
    schema.assign("$ref", ref_value);
  }
};
