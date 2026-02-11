class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Setting `$ref` inside a single-entry `allOf` is unnecessary "
            "in 2019-09 and later, as `$ref` no longer overrides sibling "
            "keywords"} {};

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

    const auto &allof = schema.at("allOf");
    std::size_t ref_branch_count = 0;
    bool has_extractable_ref = false;
    for (const auto &branch : allof.as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_branch_count++;
        if (branch.size() == 1) {
          has_extractable_ref = true;
        }
      }
    }

    return ref_branch_count == 1 && has_extractable_ref;
  }

  auto transform(JSON &schema) const -> void override {
    auto collection = schema.at("allOf");
    const auto size = collection.size();
    for (std::size_t index = 0; index < size; index++) {
      if (collection.at(index).is_object() &&
          collection.at(index).defines("$ref") &&
          collection.at(index).size() == 1) {
        schema.assign("$ref", collection.at(index).at("$ref"));
        collection.erase(std::next(collection.as_array().cbegin(),
                                   static_cast<std::ptrdiff_t>(index)));
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
