class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a `$ref` in `allOf` is unnecessary in 2019-09 and "
            "later, as `$ref` no longer overrides sibling keywords"} {};

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

    const auto &all_of{schema.at("allOf")};
    std::size_t ref_count{0};
    for (const auto &entry : all_of.as_array()) {
      if (entry.is_object() && entry.defines("$ref") && entry.size() == 1) {
        ref_count++;
      }
    }

    return ref_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    JSON ref_value{JSON{nullptr}};
    for (const auto &entry : schema.at("allOf").as_array()) {
      if (entry.is_object() && entry.defines("$ref") && entry.size() == 1) {
        ref_value = entry.at("$ref");
        break;
      }
    }

    auto &all_of{schema.at("allOf")};
    for (auto it = all_of.as_array().cbegin(); it != all_of.as_array().cend();
         ++it) {
      if (it->is_object() && it->defines("$ref") && it->size() == 1) {
        all_of.erase(it);
        break;
      }
    }

    if (all_of.empty()) {
      schema.erase("allOf");
    }

    schema.assign("$ref", std::move(ref_value));
  }
};
