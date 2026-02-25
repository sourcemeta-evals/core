class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a `$ref` in `allOf` is unnecessary in JSON Schema "
            "draft 2019-09 and later, unless multiple `allOf` branches contain "
            "`$ref`"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    return contains_any(
               vocabularies,
               {"https://json-schema.org/draft/2020-12/vocab/applicator",
                "https://json-schema.org/draft/2019-09/vocab/applicator"}) &&
           schema.is_object() && !schema.defines("$ref") &&
           schema.defines("allOf") && schema.at("allOf").is_array() &&
           std::count_if(
               schema.at("allOf").as_array().cbegin(),
               schema.at("allOf").as_array().cend(), [](const auto &subschema) {
                 return subschema.is_object() && subschema.defines("$ref");
               }) == 1;
  }

  auto transform(JSON &schema) const -> void override {
    const auto all_of = schema.at("allOf");
    auto remaining = sourcemeta::core::JSON::make_array();
    for (const auto &subschema : all_of.as_array()) {
      if (subschema.is_object() && subschema.defines("$ref")) {
        schema.assign("$ref", subschema.at("$ref"));
        if (subschema.size() > 1) {
          auto copy = subschema;
          copy.erase("$ref");
          remaining.push_back(std::move(copy));
        }
      } else {
        remaining.push_back(subschema);
      }
    }

    if (remaining.empty()) {
      schema.erase("allOf");
    } else {
      schema.assign("allOf", std::move(remaining));
    }
  }
};
