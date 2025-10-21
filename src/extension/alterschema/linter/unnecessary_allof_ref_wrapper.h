class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Unnecessary `allOf` wrapper around `$ref` in modern JSON Schema "
            "versions"} {};

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
        schema.defines("$ref")) {
      return false;
    }

    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.empty()) {
      return false;
    }

    std::size_t ref_count = 0;
    for (const auto &branch : allof.as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
        if (ref_count > 1) {
          return false;
        }
      }
    }

    return ref_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto allof = schema.at("allOf");

    std::size_t ref_index = 0;
    for (std::size_t i = 0; i < allof.size(); i++) {
      if (allof.at(i).is_object() && allof.at(i).defines("$ref")) {
        ref_index = i;
        break;
      }
    }

    auto ref_value = allof.at(ref_index).at("$ref");
    schema.assign("$ref", std::move(ref_value));

    allof.at(ref_index).erase("$ref");

    if (allof.at(ref_index).is_object() && allof.at(ref_index).empty()) {
      allof.erase(allof.as_array().begin() +
                  static_cast<JSON::Array::difference_type>(ref_index));
    }

    if (allof.empty()) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(allof));
    }
  }
};
