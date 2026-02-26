class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "In JSON Schema 2019-09 and later, `$ref` can have sibling "
            "keywords, so wrapping it in `allOf` is unnecessary"} {};

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

    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array() || schema.at("allOf").empty()) {
      return false;
    }

    // Don't apply if there is already a $ref sibling to allOf
    if (schema.defines("$ref")) {
      return false;
    }

    // Count how many branches in allOf contain $ref
    std::size_t ref_count{0};
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
      }
    }

    // Only apply if exactly one branch contains $ref and that branch
    // is solely a $ref (no other keywords)
    if (ref_count != 1) {
      return false;
    }

    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref") && branch.size() == 1) {
        return true;
      }
    }

    return false;
  }

  auto transform(JSON &schema) const -> void override {
    // Find the branch with $ref, extract it, and rebuild allOf without it
    auto collection = schema.at("allOf");
    JSON new_allof = JSON::make_array();
    for (const auto &branch : collection.as_array()) {
      if (branch.is_object() && branch.defines("$ref") && branch.size() == 1 &&
          !schema.defines("$ref")) {
        schema.assign("$ref", branch.at("$ref"));
      } else {
        new_allof.push_back(branch);
      }
    }

    if (new_allof.empty()) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(new_allof));
    }
  }
};
