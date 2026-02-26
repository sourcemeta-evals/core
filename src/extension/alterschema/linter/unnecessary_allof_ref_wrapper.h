class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a single `$ref` branch in `allOf` is unnecessary in "
            "JSON Schema 2019-09 and later"} {};

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

    std::size_t references{0};
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        references += 1;
        if (references > 1) {
          return false;
        }
      }
    }

    return references == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto branches{sourcemeta::core::JSON::make_array()};
    const auto all_of{schema.at("allOf")};
    bool extracted{false};

    for (const auto &branch : all_of.as_array()) {
      if (!extracted && branch.is_object() && branch.defines("$ref")) {
        schema.assign("$ref", branch.at("$ref"));
        extracted = true;

        auto residual{branch};
        residual.erase("$ref");
        if (!residual.empty()) {
          branches.push_back(std::move(residual));
        }
      } else {
        branches.push_back(branch);
      }
    }

    if (branches.empty()) {
      schema.erase("allOf");
    } else {
      schema.assign("allOf", std::move(branches));
    }
  }
};
