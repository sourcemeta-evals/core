class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Setting `$ref` inside a single-entry `allOf` is unnecessary "
            "in 2019-09 and later, as `$ref` no longer overrides sibling "
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

    if (!schema.is_object() || !schema.defines("allOf") ||
        !schema.at("allOf").is_array() || schema.defines("$ref")) {
      return false;
    }

    const auto &all_of{schema.at("allOf")};
    std::size_t ref_branch_count{0};
    bool has_ref_only_branch{false};
    for (const auto &branch : all_of.as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_branch_count++;
        if (branch.size() == 1) {
          has_ref_only_branch = true;
        }
      }
    }

    return ref_branch_count == 1 && has_ref_only_branch;
  }

  auto transform(JSON &schema) const -> void override {
    auto collection = schema.at("allOf");
    auto new_all_of{JSON::make_array()};
    for (auto &branch : collection.as_array()) {
      if (branch.is_object() && branch.defines("$ref") && branch.size() == 1) {
        schema.assign("$ref", branch.at("$ref"));
      } else {
        new_all_of.push_back(std::move(branch));
      }
    }

    if (new_all_of.empty()) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(new_all_of));
    }
  }
};
