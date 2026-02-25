class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a single `$ref` branch in `allOf` is unnecessary in "
            "JSON Schema 2019-09 and newer"} {};

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

    const auto ref_branches{std::count_if(
        schema.at("allOf").as_array().cbegin(),
        schema.at("allOf").as_array().cend(), [](const auto &subschema) {
          return subschema.is_object() && subschema.defines("$ref");
        })};
    const auto wrapper_branches{std::count_if(
        schema.at("allOf").as_array().cbegin(),
        schema.at("allOf").as_array().cend(), [](const auto &subschema) {
          return subschema.is_object() && subschema.size() == 1 &&
                 subschema.defines("$ref");
        })};

    return ref_branches == 1 && wrapper_branches == 1;
  }

  auto transform(JSON &schema) const -> void override {
    const auto original_all_of{schema.at("allOf")};
    auto all_of{sourcemeta::core::JSON::make_array()};
    auto reference{sourcemeta::core::JSON{nullptr}};

    for (const auto &subschema : original_all_of.as_array()) {
      if (subschema.is_object() && subschema.size() == 1 &&
          subschema.defines("$ref")) {
        reference = subschema.at("$ref");
      } else {
        all_of.push_back(subschema);
      }
    }

    schema.assign("$ref", std::move(reference));

    if (all_of.empty()) {
      schema.erase("allOf");
    } else {
      schema.assign("allOf", std::move(all_of));
    }
  }
};
