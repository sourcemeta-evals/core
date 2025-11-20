class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` can have sibling "
            "keywords, so wrapping it in `allOf` is unnecessary"} {};

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
        !schema.at("allOf").is_array() || schema.at("allOf").empty()) {
      return false;
    }

    if (schema.defines("$ref")) {
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

    auto ref_branch_iter = collection.as_array().begin();
    for (auto it = collection.as_array().begin();
         it != collection.as_array().end(); ++it) {
      if (it->is_object() && it->defines("$ref") && it->size() == 1) {
        ref_branch_iter = it;
        break;
      }
    }

    auto ref_value = ref_branch_iter->at("$ref");
    schema.assign("$ref", std::move(ref_value));

    collection.erase(ref_branch_iter, std::next(ref_branch_iter));

    if (collection.empty()) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(collection));
    }
  }
};
