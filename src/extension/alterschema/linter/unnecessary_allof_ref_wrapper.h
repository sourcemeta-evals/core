class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping $ref in allOf is unnecessary in modern JSON Schema "
            "versions where $ref can have siblings"} {};

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
           schema.at("allOf").is_array() && !schema.defines("$ref") &&
           this->has_single_ref_only_branch(schema.at("allOf").as_array());
  }

  auto transform(JSON &schema) const -> void override {
    auto allof = schema.at("allOf");

    for (auto it = allof.as_array().begin(); it != allof.as_array().end();
         ++it) {
      if (it->is_object() && it->defines("$ref") && it->size() == 1) {
        schema.assign("$ref", it->at("$ref"));
        allof.erase(it, std::next(it));
        break;
      }
    }

    if (allof.as_array().begin() == allof.as_array().end()) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(allof));
    }
  }

private:
  auto has_single_ref_only_branch(const JSON::Array &branches) const -> bool {
    int ref_count = 0;
    bool has_ref_only = false;

    for (const auto &branch : branches) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
        if (branch.size() == 1) {
          has_ref_only = true;
        }
      }
    }

    return ref_count == 1 && has_ref_only;
  }
};
