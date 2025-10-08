#ifndef SOURCEMETA_CORE_ALTERSCHEMA_LINTER_UNNECESSARY_ALLOF_REF_WRAPPER_H_
#define SOURCEMETA_CORE_ALTERSCHEMA_LINTER_UNNECESSARY_ALLOF_REF_WRAPPER_H_

class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "The `allOf` wrapper around a single `$ref` is unnecessary in "
            "2019-09 and later, as `$ref` no longer overrides sibling "
            "keywords"} {};

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

    if (!schema.is_object() || !schema.defines("allOf")) {
      return false;
    }

    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.size() != 1) {
      return false;
    }

    const auto &branch = allof.at(0);
    if (!branch.is_object() || !branch.defines("$ref") || branch.size() != 1) {
      return false;
    }

    return !schema.defines("$ref");
  }

  auto transform(JSON &schema) const -> void override {
    auto ref_value = schema.at("allOf").at(0).at("$ref");
    schema.erase("allOf");
    schema.assign("$ref", std::move(ref_value));
  }
};

#endif
