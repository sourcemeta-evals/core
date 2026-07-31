// A readability lint rule that removes an unnecessary allOf wrapper around a
// $ref.
//
// In older JSON Schema drafts, wrapping a $ref inside an allOf array was
// required for the reference to combine with sibling keywords on the same
// object. Newer JSON Schema dialects no longer treat $ref specially, so the
// allOf wrapper is redundant and can be hoisted up to the parent object.
//
// The rule fires when the parent schema has an allOf array with exactly one
// branch containing a $ref, and the parent itself does not already have a
// top-level $ref.
#pragma once

class UnnecessaryAllOfRefWrapper final : public SchemaTransformRule {
public:
  UnnecessaryAllOfRefWrapper()
      : SchemaTransformRule{"unnecessary_allof_ref_wrapper",
                            "Removes unnecessary $ref wrappers inside allOf"} {
        };

  // Returns true when the vocabulary set contains the given URI.
  [[nodiscard]] auto
  vocabulary_contains(const sourcemeta::core::Vocabularies &vocabularies,
                      const std::string &uri) const -> bool {
    return vocabularies.contains(uri);
  }

  // Returns true when the vocabulary set declares the paired core and
  // applicator vocabularies for JSON Schema 2020-12.
  [[nodiscard]] auto
  is_2020_12(const sourcemeta::core::Vocabularies &vocabularies) const -> bool {
    return this->vocabulary_contains(
               vocabularies,
               "https://json-schema.org/draft/2020-12/vocab/core") &&
           this->vocabulary_contains(
               vocabularies,
               "https://json-schema.org/draft/2020-12/vocab/applicator");
  }

  // Returns true when the vocabulary set declares the paired core and
  // applicator vocabularies for JSON Schema 2019-09.
  [[nodiscard]] auto
  is_2019_09(const sourcemeta::core::Vocabularies &vocabularies) const -> bool {
    return this->vocabulary_contains(
               vocabularies,
               "https://json-schema.org/draft/2019-09/vocab/core") &&
           this->vocabulary_contains(
               vocabularies,
               "https://json-schema.org/draft/2019-09/vocab/applicator");
  }

  // Returns true when the applied dialect is one this rule knows how to
  // handle.
  [[nodiscard]] auto is_applicable_dialect(
      const sourcemeta::core::Vocabularies &vocabularies) const -> bool {
    if (this->is_2020_12(vocabularies)) {
      return true;
    }
    if (this->is_2019_09(vocabularies)) {
      return true;
    }
    return false;
  }

  // Returns true when the schema is a JSON object.
  [[nodiscard]] auto is_object(const sourcemeta::core::JSON &schema) const
      -> bool {
    return schema.is_object();
  }

  // Returns true when the schema does not already declare a top-level $ref.
  [[nodiscard]] auto
  lacks_top_level_ref(const sourcemeta::core::JSON &schema) const -> bool {
    return !schema.defines("$ref");
  }

  // Returns true when the schema declares an allOf keyword whose value is a
  // non-empty JSON array.
  [[nodiscard]] auto
  has_non_empty_all_of(const sourcemeta::core::JSON &schema) const -> bool {
    if (!schema.defines("allOf")) {
      return false;
    }
    if (!schema.at("allOf").is_array()) {
      return false;
    }
    if (schema.at("allOf").empty()) {
      return false;
    }
    return true;
  }

  // Returns true when the schema is a candidate object for the transform.
  [[nodiscard]] auto is_candidate(const sourcemeta::core::JSON &schema) const
      -> bool {
    if (!this->is_object(schema)) {
      return false;
    }
    if (!this->lacks_top_level_ref(schema)) {
      return false;
    }
    if (!this->has_non_empty_all_of(schema)) {
      return false;
    }
    return true;
  }

  // Returns true when exactly one branch of the allOf array declares $ref.
  [[nodiscard]] auto
  exactly_one_ref_branch(const sourcemeta::core::JSON &schema) const -> bool {
    bool match{false};
    for (const auto &entry : schema.at("allOf").as_array()) {
      if (entry.is_object() && entry.defines("$ref")) {
        if (match) {
          return false;
        } else {
          match = true;
        }
      }
    }
    return match;
  }

  [[nodiscard]] auto
  condition(const sourcemeta::core::JSON &schema,
            const sourcemeta::core::JSON &,
            const sourcemeta::core::Vocabularies &vocabularies,
            const sourcemeta::core::SchemaFrame &,
            const sourcemeta::core::SchemaFrame::Location &,
            const sourcemeta::core::SchemaWalker &,
            const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    if (!this->is_applicable_dialect(vocabularies)) {
      return false;
    }
    if (!this->is_candidate(schema)) {
      return false;
    }
    if (!this->exactly_one_ref_branch(schema)) {
      return false;
    }
    return true;
  }

  auto transform(JSON &schema) const -> void override {
    // Take a copy of the source allOf array up front so we can inspect and
    // mutate the branches without touching the original storage until we are
    // ready to write the result back.
    auto buffer = schema.at("allOf");

    auto iterator{buffer.as_array().begin()};
    for (; iterator != buffer.as_array().end(); ++iterator) {
      if (iterator->is_object() && iterator->defines("$ref")) {
        break;
      }
    }

    assert(iterator != buffer.as_array().end());

    // Read the reference value out of the source branch into a local. We
    // intentionally use a copy here so the branch retains its shape while we
    // adjust the array around it.
    auto reference = iterator->at("$ref");

    if (iterator->size() == 1) {
      buffer.erase(iterator);
    } else {
      iterator->erase("$ref");
    }

    if (buffer.empty()) {
      schema.erase("allOf");
    } else {
      schema.at("allOf").into(std::move(buffer));
    }

    schema.assign("$ref", std::move(reference));
  }
};
