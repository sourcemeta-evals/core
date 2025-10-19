class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{
            "unnecessary_allof_ref_wrapper",
            "Wrapping a `$ref` in `allOf` is unnecessary in modern JSON Schema "
            "versions (2019-09 and later), as `$ref` no longer overrides "
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
    if (!contains_any(vocabularies,
                      {"https://json-schema.org/draft/2020-12/vocab/core",
                       "https://json-schema.org/draft/2019-09/vocab/core"})) {
      return false;
    }

    if (!schema.is_object() || !schema.defines("allOf") ||
        schema.defines("$ref")) {
      return false;
    }

    const auto &allof = schema.at("allOf");
    if (!allof.is_array() || allof.empty()) {
      return false;
    }

    std::size_t ref_count = 0;
    std::optional<std::size_t> ref_only_index;

    for (std::size_t i = 0; i < allof.size(); ++i) {
      const auto &branch = allof.at(i);
      if (branch.is_object() && branch.defines("$ref")) {
        ref_count++;
        if (branch.size() == 1) {
          ref_only_index = i;
        }
      }
    }

    return ref_count == 1 && ref_only_index.has_value();
  }

  auto transform(JSON &schema) const -> void override {
    const auto &allof = schema.at("allOf");
    std::optional<std::size_t> ref_only_index;

    for (std::size_t i = 0; i < allof.size(); ++i) {
      const auto &branch = allof.at(i);
      if (branch.is_object() && branch.defines("$ref") && branch.size() == 1) {
        ref_only_index = i;
        break;
      }
    }

    if (!ref_only_index.has_value()) {
      return;
    }

    auto ref_value = allof.at(ref_only_index.value()).at("$ref");
    schema.assign("$ref", ref_value);

    auto collection = schema.at("allOf");
    collection.erase(collection.as_array().begin() +
                     static_cast<std::ptrdiff_t>(ref_only_index.value()));

    if (collection.empty()) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(collection));
    }
  }
};
