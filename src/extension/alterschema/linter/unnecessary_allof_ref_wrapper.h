class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{"unnecessary_allof_ref_wrapper",
                            "Wrapping `$ref` in `allOf` is unnecessary in JSON "
                            "Schema 2019-09 and later versions as `$ref` does "
                            "not override sibling keywords anymore"} {};

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &frame,
            const sourcemeta::core::SchemaFrame::Location &location,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    if (!((vocabularies.contains(
               "https://json-schema.org/draft/2020-12/vocab/core") &&
           vocabularies.contains(
               "https://json-schema.org/draft/2020-12/vocab/applicator")) ||
          (vocabularies.contains(
               "https://json-schema.org/draft/2019-09/vocab/core") &&
           vocabularies.contains(
               "https://json-schema.org/draft/2019-09/vocab/applicator")))) {
      return false;
    }

    if (!schema.is_object() || schema.defines("$ref") ||
        !schema.defines("allOf") || !schema.at("allOf").is_array() ||
        schema.at("allOf").empty()) {
      return false;
    }

    bool match{false};
    for (const auto &entry : schema.at("allOf").as_array()) {
      if (entry.is_object() && entry.defines("$ref")) {
        if (match ||
            entry.defines_any({"$id", "$anchor", "$dynamicAnchor",
                               "$recursiveAnchor", "unevaluatedProperties",
                               "unevaluatedItems"})) {
          return false;
        }

        match = true;
      }
    }

    if (!match) {
      return false;
    }

    // Hoisting rewrites the array, so a reference that lands inside it would
    // either address a different branch afterwards or address nothing at all
    const auto wrapper{
        location.pointer.concat(sourcemeta::core::Pointer{"allOf"})};
    for (const auto &reference : frame.references()) {
      const auto target{frame.traverse(reference.second.destination)};
      if (target.has_value() &&
          target.value().get().pointer.starts_with(wrapper)) {
        return false;
      }
    }

    return true;
  }

  auto transform(JSON &schema) const -> void override {
    auto &array{schema.at("allOf").as_array()};
    auto iterator{array.begin()};
    for (; iterator != array.end(); ++iterator) {
      if (iterator->is_object() && iterator->defines("$ref")) {
        break;
      }
    }

    assert(iterator != array.end());
    auto reference{std::move(iterator->at("$ref"))};
    if (iterator->size() == 1) {
      schema.at("allOf").erase(iterator);
      if (schema.at("allOf").empty()) {
        schema.erase("allOf");
      }
    } else {
      iterator->erase("$ref");
    }

    schema.assign("$ref", std::move(reference));
  }
};
