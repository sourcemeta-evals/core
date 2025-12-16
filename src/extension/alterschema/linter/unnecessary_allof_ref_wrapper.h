class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` no longer overrides "
            "sibling keywords, so wrapping `$ref` in `allOf` is unnecessary"} {
        };

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

    const auto &all_of = schema.at("allOf");
    std::size_t ref_branch_count = 0;

    for (const auto &branch : all_of.as_array()) {
      if (branch.is_object() && branch.defines("$ref") && branch.size() == 1) {
        ref_branch_count++;
      }
    }

    return ref_branch_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto all_of = schema.at("allOf");
    std::size_t ref_branch_index = 0;

    for (std::size_t i = 0; i < all_of.size(); ++i) {
      const auto &branch = all_of.at(i);
      if (branch.is_object() && branch.defines("$ref") && branch.size() == 1) {
        ref_branch_index = i;
        break;
      }
    }

    auto ref_value = all_of.at(ref_branch_index).at("$ref");
    schema.assign("$ref", std::move(ref_value));

    if (all_of.size() == 1) {
      schema.erase("allOf");
    } else {
      all_of.erase(all_of.as_array().begin() +
                       static_cast<std::ptrdiff_t>(ref_branch_index),
                   all_of.as_array().begin() +
                       static_cast<std::ptrdiff_t>(ref_branch_index) + 1);
      schema.at("allOf").into(std::move(all_of));
    }
  }
};
