class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a single `$ref` branch in `allOf` is unnecessary in "
            "modern JSON Schema drafts"} {};

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

    const auto &branches{schema.at("allOf").as_array()};
    const auto references{std::count_if(
        branches.cbegin(), branches.cend(), [](const auto &subschema) {
          return subschema.is_object() && subschema.defines("$ref");
        })};

    const auto wrappers{std::count_if(
        branches.cbegin(), branches.cend(), [](const auto &subschema) {
          return subschema.is_object() && subschema.size() == 1 &&
                 subschema.defines("$ref");
        })};

    return references == 1 && wrappers == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto collection{schema.at("allOf")};
    auto &branches{collection.as_array()};
    auto iterator = std::find_if(
        branches.begin(), branches.end(), [](const auto &subschema) {
          return subschema.is_object() && subschema.size() == 1 &&
                 subschema.defines("$ref");
        });

    assert(iterator != branches.end());

    auto reference{iterator->at("$ref")};
    collection.erase(iterator, std::next(iterator));

    schema.assign("$ref", std::move(reference));

    if (collection.empty()) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(collection));
    }
  }
};
