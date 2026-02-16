class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Setting `$ref` inside `allOf` is unnecessary starting with "
            "2019-09, as `$ref` no longer overrides sibling keywords"} {};

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

    std::size_t ref_branch_count = 0;
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref") && branch.size() == 1) {
        ref_branch_count++;
      }
    }

    return ref_branch_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto collection = schema.at("allOf");
    for (std::size_t index = 0; index < collection.size(); ++index) {
      const auto &branch = collection.at(index);
      if (branch.is_object() && branch.defines("$ref") && branch.size() == 1) {
        schema.assign("$ref", branch.at("$ref"));
        collection.erase(collection.as_array().begin() +
                         static_cast<std::ptrdiff_t>(index));
        break;
      }
    }

    if (collection.empty()) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(collection));
    }
  }
};
