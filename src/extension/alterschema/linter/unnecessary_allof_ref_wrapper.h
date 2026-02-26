class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a `$ref` in `allOf` is unnecessary in modern JSON Schema "
            "drafts when there is no sibling `$ref`"} {};

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
             "https://json-schema.org/draft/2019-09/vocab/applicator"}) ||
        !schema.is_object() || schema.defines("$ref") ||
        !schema.defines("allOf") || !schema.at("allOf").is_array()) {
      return false;
    }

    std::size_t references{0};
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.size() == 1 && branch.defines("$ref")) {
        references += 1;
        if (references > 1) {
          return false;
        }
      }
    }

    return references == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto &all_of{schema.at("allOf")};
    auto iterator =
        std::find_if(all_of.as_array().begin(), all_of.as_array().end(),
                     [](const auto &branch) {
                       return branch.is_object() && branch.size() == 1 &&
                              branch.defines("$ref");
                     });

    assert(iterator != all_of.as_array().end());
    const auto reference{iterator->at("$ref")};
    all_of.erase(iterator);
    if (all_of.empty()) {
      schema.erase("allOf");
    }
    schema.assign("$ref", reference);
  }
};
