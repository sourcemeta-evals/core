class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` can have sibling "
            "keywords, so wrapping it in `allOf` is no longer necessary"} {};

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

    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array() || schema.defines("$ref")) {
      return false;
    }

    const auto &all_of = schema.at("allOf");
    std::size_t ref_branch_count = 0;
    bool has_pure_ref_branch = false;

    for (const auto &branch : all_of.as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_branch_count++;
        if (branch.size() == 1) {
          has_pure_ref_branch = true;
        }
      }
    }

    return ref_branch_count == 1 && has_pure_ref_branch;
  }

  auto transform(JSON &schema) const -> void override {
    auto all_of = schema.at("allOf");
    std::size_t ref_index = 0;

    for (std::size_t i = 0; i < all_of.size(); ++i) {
      const auto &branch = all_of.at(i);
      if (branch.is_object() && branch.defines("$ref") && branch.size() == 1) {
        ref_index = i;
        break;
      }
    }

    auto ref_value = all_of.at(ref_index).at("$ref");
    schema.assign("$ref", std::move(ref_value));

    all_of.erase(all_of.as_array().begin() +
                 static_cast<std::ptrdiff_t>(ref_index));

    if (all_of.empty()) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(all_of));
    }
  }
};
