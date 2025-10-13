class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a `$ref` in `allOf` is unnecessary in modern JSON Schema "
            "versions (2019-09 and later) where `$ref` no longer overrides "
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
    if (!contains_any(
            vocabularies,
            {"https://json-schema.org/draft/2020-12/vocab/applicator",
             "https://json-schema.org/draft/2019-09/vocab/applicator"})) {
      return false;
    }

    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array()) {
      return false;
    }

    if (schema.defines("$ref")) {
      return false;
    }

    const auto &allof = schema.at("allOf");
    std::size_t ref_only_count = 0;
    for (const auto &branch : allof.as_array()) {
      if (branch.is_object() && branch.size() == 1 && branch.defines("$ref")) {
        ref_only_count++;
      }
    }

    return ref_only_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto &allof = schema.at("allOf");
    for (const auto &branch : allof.as_array()) {
      if (branch.is_object() && branch.size() == 1 && branch.defines("$ref")) {
        schema.assign("$ref", branch.at("$ref"));
        break;
      }
    }

    schema.erase("allOf");
  }
};
