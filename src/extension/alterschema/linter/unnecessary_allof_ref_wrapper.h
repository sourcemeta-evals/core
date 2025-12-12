class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` no longer overrides "
            "sibling keywords, so wrapping a `$ref` in `allOf` is unnecessary "
            "when there is only one `$ref` among the branches"} {};

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
      if (branch.is_object() && branch.defines("$ref")) {
        ref_branch_count++;
      }
    }

    if (ref_branch_count != 1) {
      return false;
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    auto allof = schema.at("allOf");

    std::size_t ref_branch_index = 0;
    std::size_t index = 0;
    for (const auto &branch : allof.as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_branch_index = index;
        break;
      }
      index++;
    }

    auto it = allof.as_array().begin();
    std::advance(it, static_cast<std::ptrdiff_t>(ref_branch_index));
    auto ref_value = it->at("$ref");

    if (it->size() == 1) {
      if (allof.size() == 1) {
        schema.assign("$ref", std::move(ref_value));
        schema.erase("allOf");
      } else {
        schema.assign("$ref", std::move(ref_value));
        allof.erase(it, std::next(it));
        schema.at("allOf").into(std::move(allof));
      }
    } else {
      schema.assign("$ref", std::move(ref_value));
      auto modified_branch = *it;
      modified_branch.erase("$ref");
      *it = std::move(modified_branch);
      schema.at("allOf").into(std::move(allof));
    }
  }
};
