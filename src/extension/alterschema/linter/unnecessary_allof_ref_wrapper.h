class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{"unnecessary_allof_ref_wrapper",
                            "Unnecessary `allOf` wrapper around `$ref` can be "
                            "removed in modern "
                            "drafts"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    const bool is_modern = contains_any(
        vocabularies, {"https://json-schema.org/draft/2020-12/vocab/core",
                       "https://json-schema.org/draft/2019-09/vocab/core"});
    const bool has_applicator = contains_any(
        vocabularies,
        {"https://json-schema.org/draft/2020-12/vocab/applicator",
         "https://json-schema.org/draft/2019-09/vocab/applicator"});

    if (!is_modern || !has_applicator)
      return false;

    if (!schema.is_object() || !schema.defines("allOf") ||
        schema.defines("$ref"))
      return false;

    const auto &all_of = schema.at("allOf");
    if (!all_of.is_array())
      return false;

    int ref_count = 0;
    for (const auto &branch : all_of.as_array()) {
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
      }
    }

    return ref_count == 1;
  }

  auto transform(JSON &schema) const -> void override {
    auto collection = schema.at("allOf");
    auto &array = collection.as_array();

    // Find the branch with $ref
    auto it = std::find_if(array.begin(), array.end(), [](const JSON &branch) {
      return branch.is_object() && branch.defines("$ref");
    });

    if (it != array.end()) {
      // Move $ref to parent
      schema.assign("$ref", std::move(it->at("$ref")));

      // Remove $ref from branch
      it->erase("$ref");

      // If branch is empty, remove it
      if (it->as_object().size() == 0) {
        collection.erase(it);
      }

      // If allOf is empty, remove it
      if (array.begin() == array.end()) {
        schema.erase("allOf");
      } else {
        schema.at("allOf").into(std::move(collection));
      }
    }
  }
};
