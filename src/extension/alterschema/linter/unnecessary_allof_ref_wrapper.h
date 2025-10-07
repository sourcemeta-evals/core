class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping `$ref` in `allOf` is unnecessary in modern JSON Schema "
            "versions (2019-09 and later) where `$ref` no longer overrides "
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
        !schema.at("allOf").is_array()) {
      return false;
    }

    if (schema.defines("$ref")) {
      return false;
    }

    std::size_t ref_count = 0;
    for (const auto &branch : schema.at("allOf").as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
      }
    }

    return ref_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto collection = schema.at("allOf");
    auto &allof_array = collection.as_array();

    auto ref_branch_it = std::find_if(
        allof_array.begin(), allof_array.end(), [](const auto &branch) {
          return branch.is_object() && branch.defines("$ref");
        });

    assert(ref_branch_it != allof_array.end());

    for (const auto &entry : ref_branch_it->as_object()) {
      schema.assign(entry.first, entry.second);
    }

    collection.erase(ref_branch_it, std::next(ref_branch_it));

    if (collection.size() == 0) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(collection));
    }
  }
};
