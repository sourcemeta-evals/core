class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In modern JSON Schema versions (2019-09 and later), `$ref` no "
            "longer overrides sibling keywords, making `allOf` wrappers "
            "around a single `$ref` unnecessary"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    if (!contains_any(vocabularies,
                      {"https://json-schema.org/draft/2020-12/vocab/core",
                       "https://json-schema.org/draft/2019-09/vocab/core"})) {
      return false;
    }

    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    if (schema.defines("$ref")) {
      return false;
    }

    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.size() != 1) {
      return false;
    }

    const auto &branch = allof.at(0);
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    std::size_t ref_count = 0;
    for (const auto &item : allof.as_array()) {
      if (item.is_object() && item.defines("$ref")) {
        ref_count++;
      }
    }

    return ref_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto allof = schema.at("allOf");
    auto branch = allof.at(0);
    schema.erase("allOf");
    for (const auto &entry : branch.as_object()) {
      schema.assign(entry.first, entry.second);
    }
  }
};
