class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a single `allOf` branch containing `$ref` is unnecessary "
            "in JSON Schema 2019-09 and later, because `$ref` no longer "
            "overrides sibling keywords"} {};

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
           std::count_if(schema.at("allOf").as_array().cbegin(),
                         schema.at("allOf").as_array().cend(),
                         [](const auto &branch) {
                           return branch.is_object() && branch.defines("$ref");
                         }) == 1;
  }

  auto transform(JSON &schema) const -> void override {
    const auto all_of{schema.at("allOf")};
    auto branches{sourcemeta::core::JSON::make_array()};
    auto extracted{false};

    for (auto branch : all_of.as_array()) {
      if (!extracted && branch.is_object() && branch.defines("$ref")) {
        schema.assign("$ref", branch.at("$ref"));
        branch.erase("$ref");
        extracted = true;

        if (!branch.empty()) {
          branches.push_back(std::move(branch));
        }
      } else {
        branches.push_back(std::move(branch));
      }
    }

    if (branches.empty()) {
      schema.erase("allOf");
    } else {
      schema.assign("allOf", std::move(branches));
    }
  }
};
