class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Unnecessary `allOf` wrapper around `$ref` in modern JSON Schema "
            "versions (2019-09+)"} {};

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

    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    if (schema.defines("$ref")) {
      return false;
    }

    if (!schema.at("allOf").is_array()) {
      return false;
    }

    std::size_t ref_count = 0;
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
      }
    }

    return ref_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto allof = schema.at("allOf");

    for (auto it = allof.as_array().begin(); it != allof.as_array().end();
         ++it) {
      if (it->is_object() && it->defines("$ref")) {
        for (const auto &pair : it->as_object()) {
          schema.assign(pair.first, pair.second);
        }

        allof.erase(it);
        break;
      }
    }

    if (allof.size() == 0) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(allof));
    }
  }
};
