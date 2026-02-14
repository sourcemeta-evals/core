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
        !schema.at("allOf").is_array() || schema.at("allOf").empty()) {
      return false;
    }

    if (schema.defines("$ref")) {
      return false;
    }

    std::size_t ref_branch_count = 0;
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_branch_count++;
      }
    }

    return ref_branch_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    const auto &allof = schema.at("allOf");
    const auto allof_size = allof.size();

    std::size_t ref_index = 0;
    for (std::size_t i = 0; i < allof_size; ++i) {
      if (allof.at(i).is_object() && allof.at(i).defines("$ref")) {
        ref_index = i;
        break;
      }
    }

    auto ref_value = allof.at(ref_index).at("$ref");

    if (allof_size == 1) {
      schema.erase("allOf");
    } else {
      auto new_allof = sourcemeta::core::JSON::make_array();
      for (std::size_t i = 0; i < allof_size; ++i) {
        if (i != ref_index) {
          new_allof.push_back(allof.at(i));
        }
      }

      schema.assign("allOf", std::move(new_allof));
    }

    schema.assign("$ref", std::move(ref_value));
  }
};
