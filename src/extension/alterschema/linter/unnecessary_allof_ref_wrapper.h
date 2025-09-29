#include <sourcemeta/core/jsonschema.h>

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
    // Only apply to modern schema versions (2019-09 and later)
    return contains_any(
               vocabularies,
               {"https://json-schema.org/draft/2020-12/vocab/applicator",
                "https://json-schema.org/draft/2019-09/vocab/applicator"}) &&
           schema.is_object() && schema.defines("allOf") &&
           schema.at("allOf").is_array() && schema.at("allOf").size() == 1 &&
           schema.at("allOf").at(0).is_object() &&
           schema.at("allOf").at(0).defines("$ref") &&
           !has_sibling_keywords(schema);
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    // Get the branch content
    const auto branch = schema.at("allOf").at(0);

    // Remove the allOf
    schema.erase("allOf");

    // Copy the $ref property
    schema.assign("$ref", branch.at("$ref"));

    // Copy any additional properties from the branch (excluding $ref)
    for (const auto &entry : branch.as_object()) {
      if (entry.first != "$ref") {
        schema.assign(entry.first, entry.second);
      }
    }
  }

private:
  auto has_sibling_keywords(const sourcemeta::core::JSON &schema) const
      -> bool {
    for (const auto &entry : schema.as_object()) {
      if (entry.first != "allOf" && entry.first != "$schema" &&
          entry.first != "$id" && entry.first != "$anchor" &&
          entry.first != "$dynamicAnchor") {
        return true;
      }
    }
    return false;
  }
};
