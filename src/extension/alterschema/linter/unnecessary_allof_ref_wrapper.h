class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In modern JSON Schema versions (2019-09 and later), `$ref` no "
            "longer overrides sibling keywords, so wrapping it in `allOf` is "
            "unnecessary"} {};

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
        !schema.at("allOf").is_array() || schema.defines("$ref")) {
      return false;
    }

    std::size_t ref_branch_count{0};
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref") && branch.size() == 1) {
        ref_branch_count++;
      }
    }

    return ref_branch_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    JSON ref_value{JSON{nullptr}};
    for (auto it = schema.at("allOf").as_array().cbegin();
         it != schema.at("allOf").as_array().cend(); ++it) {
      if (it->is_object() && it->defines("$ref") && it->size() == 1) {
        ref_value = it->at("$ref");
        schema.at("allOf").erase(it);
        break;
      }
    }

    schema.assign("$ref", std::move(ref_value));

    if (schema.at("allOf").empty()) {
      schema.erase("allOf");
    }
  }
};
