class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a single `allOf` branch that contains `$ref` is "
            "unnecessary in JSON Schema 2019-09 and later"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    if (!(vocabularies.contains(
              "https://json-schema.org/draft/2020-12/vocab/core") ||
          vocabularies.contains(
              "https://json-schema.org/draft/2019-09/vocab/core"))) {
      return false;
    }

    if (!schema.is_object() || schema.defines("$ref") ||
        !schema.defines("allOf") || !schema.at("allOf").is_array()) {
      return false;
    }

    auto references{0};
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ++references;
        if (references > 1) {
          return false;
        }
      }
    }

    return references == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto all_of = schema.at("allOf");
    auto rewritten{sourcemeta::core::JSON::make_array()};
    auto extracted{false};

    for (const auto &branch : all_of.as_array()) {
      if (!extracted && branch.is_object() && branch.defines("$ref")) {
        schema.assign("$ref", branch.at("$ref"));
        extracted = true;

        if (branch.size() > 1) {
          auto remainder{branch};
          remainder.erase("$ref");
          if (!remainder.empty()) {
            rewritten.push_back(std::move(remainder));
          }
        }

        continue;
      }

      rewritten.push_back(branch);
    }

    if (rewritten.empty()) {
      schema.erase("allOf");
    } else {
      schema.assign("allOf", std::move(rewritten));
    }
  }
};
