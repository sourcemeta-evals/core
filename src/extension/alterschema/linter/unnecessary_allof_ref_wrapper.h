class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a `$ref` in `allOf` is unnecessary in modern JSON Schema "
            "versions where `$ref` no longer overrides sibling keywords"} {};

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

    const auto &all_of{schema.at("allOf")};
    std::size_t ref_count{0};
    for (const auto &branch : all_of.as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
      }
    }

    return ref_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    std::size_t ref_index{0};
    for (std::size_t index = 0; index < schema.at("allOf").size(); index++) {
      if (schema.at("allOf").at(index).is_object() &&
          schema.at("allOf").at(index).defines("$ref")) {
        ref_index = index;
        break;
      }
    }

    auto ref_value{schema.at("allOf").at(ref_index).at("$ref")};
    const auto allof_size{schema.at("allOf").size()};

    if (allof_size == 1) {
      schema.erase("allOf");
    } else {
      auto collection = schema.at("allOf");
      collection.erase(collection.as_array().begin() +
                       static_cast<std::ptrdiff_t>(ref_index));
      schema.at("allOf").into(std::move(collection));
    }

    schema.assign("$ref", std::move(ref_value));
  }
};
