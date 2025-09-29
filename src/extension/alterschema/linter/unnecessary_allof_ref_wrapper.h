#ifndef SOURCEMETA_CORE_ALTERSCHEMA_LINTER_UNNECESSARY_ALLOF_REF_WRAPPER_H_
#define SOURCEMETA_CORE_ALTERSCHEMA_LINTER_UNNECESSARY_ALLOF_REF_WRAPPER_H_

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <sourcemeta/core/alterschema.h>

class UnnecessaryAllOfRefWrapper final
    : public sourcemeta::core::SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : sourcemeta::core::SchemaTransformRule{
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
    // Only apply to modern JSON Schema versions (2019-09 and later)
    if (!contains_any(
            vocabularies,
            {"https://json-schema.org/draft/2020-12/vocab/applicator",
             "https://json-schema.org/draft/2019-09/vocab/applicator"})) {
      return false;
    }

    // Must be an object with allOf
    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    // allOf must be an array
    const auto &allof = schema.at("allOf");
    if (!allof.is_array()) {
      return false;
    }

    // Must have exactly one branch
    if (allof.size() != 1) {
      return false;
    }

    // The single branch must contain $ref
    const auto &branch = allof.at(0);
    if (!branch.is_object() || !branch.defines("$ref")) {
      return false;
    }

    // Must not have $ref as sibling to allOf
    if (schema.defines("$ref")) {
      return false;
    }

    return true;
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    // Extract the single branch from allOf
    auto branch = schema.at("allOf").at(0);

    // Remove allOf
    schema.erase("allOf");

    // Merge the branch properties into the schema
    if (branch.is_object()) {
      for (const auto &entry : branch.as_object()) {
        schema.assign(entry.first, entry.second);
      }
    }
  }
};

#endif
