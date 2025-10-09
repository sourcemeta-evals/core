#ifndef SOURCEMETA_CORE_ALTERSCHEMA_LINTER_UNNECESSARY_ALLOF_REF_WRAPPER_H_
#define SOURCEMETA_CORE_ALTERSCHEMA_LINTER_UNNECESSARY_ALLOF_REF_WRAPPER_H_

class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In modern JSON Schema versions (2019-09+), $ref does not "
            "override sibling keywords, so wrapping $ref in allOf is "
            "unnecessary"} {};

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
           schema.is_object() && schema.defines("allOf") &&
           !schema.defines("$ref") && schema.at("allOf").is_array() &&
           schema.at("allOf").size() == 1 &&
           schema.at("allOf").at(0).is_object() &&
           schema.at("allOf").at(0).defines("$ref");
  }

  auto transform(JSON &schema) const -> void override {
    const auto element = schema.at("allOf").at(0);
    for (const auto &entry : element.as_object()) {
      schema.assign(entry.first, entry.second);
    }
    schema.erase("allOf");
  }
};

#endif
