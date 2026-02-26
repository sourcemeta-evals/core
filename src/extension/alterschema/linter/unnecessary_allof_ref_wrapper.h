class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Setting `$ref` inside a single-entry `allOf` is unnecessary "
            "in modern JSON Schema versions where `$ref` does not override "
            "sibling keywords"} {};

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
        !schema.at("allOf").is_array() || schema.defines("$ref")) {
      return false;
    }

    // Count how many branches contain $ref
    std::size_t ref_count = 0;
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
      }
    }

    // Only apply if exactly one branch contains $ref
    if (ref_count != 1) {
      return false;
    }

    // Find the branch with $ref and ensure it only contains $ref
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref") && branch.size() == 1) {
        return true;
      }
    }

    return false;
  }

  auto transform(JSON &schema) const -> void override {
    // Find the branch with $ref
    const auto &allof = schema.at("allOf");
    std::size_t ref_index = 0;
    for (std::size_t index = 0; index < allof.size(); index++) {
      if (allof.at(index).is_object() && allof.at(index).defines("$ref") &&
          allof.at(index).size() == 1) {
        ref_index = index;
        break;
      }
    }

    // Extract the $ref value
    auto ref_value = allof.at(ref_index).at("$ref");

    if (allof.size() == 1) {
      // Only one branch: remove allOf entirely and set $ref as sibling
      schema.erase("allOf");
    } else {
      // Multiple branches: remove the $ref branch from allOf
      schema.at("allOf").erase(
          std::next(schema.at("allOf").as_array().cbegin(),
                    static_cast<std::ptrdiff_t>(ref_index)));
    }

    schema.assign("$ref", std::move(ref_value));
  }
};
