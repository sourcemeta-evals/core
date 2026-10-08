#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>

#include <algorithm> // std::ranges::fill
#include <cstddef>   // std::size_t
#include <optional>  // std::optional, std::nullopt
#include <stdexcept> // std::runtime_error
#include <string>    // std::string

#define EXPECT_JSONLD_EXPAND_ERROR(expression, expected_code,                  \
                                   expected_pointer)                           \
  try {                                                                        \
    [[maybe_unused]] const auto result{expression};                            \
    FAIL() << "Expected JSON-LD error: " << (expected_code);                   \
  } catch (const sourcemeta::core::JSONLDError &error) {                       \
    EXPECT_STREQ(error.what(), (expected_code));                               \
    EXPECT_EQ(sourcemeta::core::to_string(error.pointer()),                    \
              (expected_pointer));                                             \
  } catch (...) {                                                              \
    FAIL() << "Expected a JSONLDError: " << (expected_code);                   \
  }

namespace {

auto remote_resolver() -> sourcemeta::core::JSONLDResolver {
  return [](const sourcemeta::core::JSON::StringView identifier)
             -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/recursive") {
      return sourcemeta::core::parse_json(
          R"({ "@context": "https://example.com/recursive" })");
    }
    if (identifier == "https://example.com/no-context") {
      return sourcemeta::core::parse_json(R"({ "foo": "bar" })");
    }
    if (identifier == "https://example.com/invalid-term") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "a": { "@id": "http://example.com/a", "@bogus": true } } })");
    }
    if (identifier == "https://example.com/throws") {
      throw std::runtime_error("network failure");
    }
    if (identifier == "https://example.com/throws-int") {
      throw 42;
    }
    if (identifier == "https://example.com/import-invalid") {
      return sourcemeta::core::parse_json(R"({ "@context": { "a": "bad" } })");
    }
    if (identifier == "https://example.com/scoped-missing") {
      return sourcemeta::core::parse_json(R"({
        "@context": {
          "p": {
            "@id": "http://example.com/p",
            "@context": "https://example.com/missing"
          }
        }
      })");
    }
    if (identifier == "https://example.com/import-language") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "@language": 42 } })");
    }
    if (identifier == "https://example.com/import-vocab") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "@vocab": 42 } })");
    }
    return std::nullopt;
  };
}

} // namespace

TEST(JSONLD_expand_error, cyclic_iri_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "term": { "@id": "term:term" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Cyclic IRI mapping", "/@context/term/@id");
}

TEST(JSONLD_expand_error, cyclic_iri_mapping_shorthand) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": "b:x", "b": "a:x" }
  })");

  // Either participating entry validly identifies a mutual cycle, as the
  // traversal order that determines which one completes it is unspecified
  try {
    [[maybe_unused]] const auto result{sourcemeta::core::jsonld_expand(input)};
    FAIL() << "Expected JSON-LD error: Cyclic IRI mapping";
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_STREQ(error.what(), "Cyclic IRI mapping");
    const auto pointer{sourcemeta::core::to_string(error.pointer())};
    EXPECT_TRUE(pointer == "/@context/a" || pointer == "/@context/b");
  } catch (...) {
    FAIL() << "Expected a JSONLDError: Cyclic IRI mapping";
  }
}

TEST(JSONLD_expand_error, invalid_term_definition_empty) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "": "http://example.com/" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition", "/@context/");
}

TEST(JSONLD_expand_error, keyword_redefinition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@type": "http://example.com/" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Keyword redefinition", "/@context/@type");
}

TEST(JSONLD_expand_error, protected_term_redefinition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "@protected": true, "a": "http://example.com/a" },
      { "a": "http://example.com/b" }
    ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Protected term redefinition", "/@context/1/a");
}

TEST(JSONLD_expand_error, invalid_protected_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@protected": "yes" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @protected value",
                             "/@context/a/@protected");
}

TEST(JSONLD_expand_error, invalid_iri_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid IRI mapping", "/@context/a/@id");
}

TEST(JSONLD_expand_error, invalid_keyword_alias) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "@context" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid keyword alias", "/@context/a/@id");
}

TEST(JSONLD_expand_error, invalid_iri_mapping_shorthand) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": "bad" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid IRI mapping", "/@context/a");
}

TEST(JSONLD_expand_error, invalid_keyword_alias_shorthand) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": "@context" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid keyword alias", "/@context/a");
}

TEST(JSONLD_expand_error, invalid_reverse_property) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": { "@reverse": "http://example.com/a", "@id": "http://example.com/b" }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property",
                             "/@context/a/@reverse");
}

TEST(JSONLD_expand_error, invalid_type_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@type": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type mapping", "/@context/a/@type");
}

TEST(JSONLD_expand_error, invalid_container_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@container": "@unknown" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/a/@container");
}

TEST(JSONLD_expand_error, invalid_language_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@language": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid language mapping",
                             "/@context/a/@language");
}

TEST(JSONLD_expand_error, invalid_prefix_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@prefix": "yes" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @prefix value", "/@context/a/@prefix");
}

TEST(JSONLD_expand_error, invalid_nest_value_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@nest": "@id" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/@context/a/@nest");
}

TEST(JSONLD_expand_error, invalid_scoped_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": {
        "@id": "http://example.com/a",
        "@context": { "b": { "@id": true } }
      }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid scoped context", "/@context/a/@context");
}

TEST(JSONLD_expand_error, invalid_local_context) {
  const auto input = sourcemeta::core::parse_json(R"({ "@context": true })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid local context", "/@context");
}

TEST(JSONLD_expand_error, invalid_version_value) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@version": 2.0 } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @version value", "/@context/@version");
}

TEST(JSONLD_expand_error, invalid_propagate_value) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@propagate": "yes" } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @propagate value",
                             "/@context/@propagate");
}

TEST(JSONLD_expand_error, invalid_import_value) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@import": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @import value", "/@context/@import");
}

TEST(JSONLD_expand_error, invalid_base_iri) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@base": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base IRI", "/@context/@base");
}

TEST(JSONLD_expand_error, invalid_vocab_mapping) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@vocab": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid vocab mapping", "/@context/@vocab");
}

TEST(JSONLD_expand_error, invalid_default_language) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@language": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid default language", "/@context/@language");
}

TEST(JSONLD_expand_error, invalid_base_direction) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@direction": "sideways" } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base direction", "/@context/@direction");
}

TEST(JSONLD_expand_error, processing_mode_conflict) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@version": 1.1 } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Processing mode conflict", "/@context/@version");
}

TEST(JSONLD_expand_error, malformed_base_with_space) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@base": "bad base" } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base IRI", "/@context/@base");
}

TEST(JSONLD_expand_error, malformed_base_with_bad_percent_escape) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@base": "https://example.com/%zz" } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base IRI", "/@context/@base");
}

TEST(JSONLD_expand_error, decimal_version_in_1_0) {
  auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": "v"
  })");
  input.at("@context")
      .assign("@version",
              sourcemeta::core::JSON{sourcemeta::core::Decimal{"1.1"}});

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Processing mode conflict", "/@context/@version");
}

TEST(JSONLD_expand_error, invalid_context_entry) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@protected": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid context entry", "/@context/@protected");
}

TEST(JSONLD_expand_error, invalid_context_entry_import) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@import": "https://example.com/ctx" } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid context entry", "/@context/@import");
}

TEST(JSONLD_expand_error, invalid_context_entry_propagate) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": { "@propagate": true } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid context entry", "/@context/@propagate");
}

TEST(JSONLD_expand_error, loading_remote_context_failed) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/missing" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Loading remote context failed", "/@context");
}

TEST(JSONLD_expand_error, invalid_remote_context) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/no-context" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid remote context", "/@context");
}

TEST(JSONLD_expand_error, recursive_context_inclusion_in_1_0) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/recursive" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver(),
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Recursive context inclusion", "/@context");
}

TEST(JSONLD_expand_error, unbounded_remote_context_chain) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    auto next{sourcemeta::core::JSON::String{identifier}};
    next += "x";
    auto document{sourcemeta::core::JSON::make_object()};
    document.assign("@context", sourcemeta::core::JSON{std::move(next)});
    return document;
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/chain" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver), "Context overflow",
      "/@context");
}

TEST(JSONLD_expand_error, context_overflow) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/recursive" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Context overflow", "/@context");
}

TEST(JSONLD_expand_error, error_inside_expansion_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": "v"
  })");
  const auto context = sourcemeta::core::parse_json(R"({
    "a": { "@id": "http://example.com/a", "@bogus": true }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, context),
                             "Invalid term definition", "");
}

TEST(JSONLD_expand_error, error_inside_wrapped_expansion_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": "v"
  })");
  const auto context = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@bogus": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, context),
                             "Invalid term definition", "");
}

TEST(JSONLD_expand_error, relative_context_without_base) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return sourcemeta::core::parse_json(
        R"({ "@context": { "p": "http://example.com/p" } })");
  };

  const auto input =
      sourcemeta::core::parse_json(R"({ "@context": "context.jsonld" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading document failed", "/@context");
  // The resolver contract only admits absolute IRIs, so even a permissive
  // resolver must never see the unresolved relative reference
  EXPECT_EQ(invocations, 0);
}

TEST(JSONLD_expand_error, relative_import_without_base) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return sourcemeta::core::parse_json(
        R"({ "@context": { "p": "http://example.com/p" } })");
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@import": "context.jsonld" } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading remote context failed", "/@context/@import");
  EXPECT_EQ(invocations, 0);
}

TEST(JSONLD_expand_error, imported_invalid_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@import": "https://example.com/import-invalid" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid IRI mapping", "/@context/@import");
}

TEST(JSONLD_expand_error, imported_invalid_mapping_in_context_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ { "@import": "https://example.com/import-invalid" } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid IRI mapping", "/@context/0/@import");
}

TEST(JSONLD_expand_error, local_override_of_imported_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@import": "https://example.com/import-invalid",
      "a": { "@id": true }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid IRI mapping", "/@context/a/@id");
}

TEST(JSONLD_expand_error, scoped_context_error_from_remote_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": "https://example.com/scoped-missing",
    "p": { "http://example.com/q": "v" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid scoped context", "/@context");
}

TEST(JSONLD_expand_error, scoped_context_error_from_local_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@context": "https://example.com/missing"
      }
    },
    "p": { "http://example.com/q": "v" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid scoped context", "/@context/p/@context");
}

TEST(JSONLD_expand_error, scoped_context_error_from_unused_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@context": "https://example.com/missing"
      }
    },
    "http://example.com/q": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid scoped context", "/@context/p/@context");
}

TEST(JSONLD_expand_error, scoped_context_without_context_entry) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@context": "https://example.com/no-context"
      }
    },
    "p": { "http://example.com/q": "v" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid scoped context", "/@context/p/@context");
}

TEST(JSONLD_expand_error, throwing_resolver) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/throws" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Loading remote context failed", "/@context");
}

TEST(JSONLD_expand_error, resolver_throwing_a_non_standard_exception) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/throws-int" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Loading remote context failed", "/@context");
}

TEST(JSONLD_expand_error,
     resolver_throwing_a_non_standard_exception_on_import) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@import": "https://example.com/throws-int" } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Loading remote context failed", "/@context/@import");
}

TEST(JSONLD_expand_error, throwing_resolver_on_import) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@import": "https://example.com/throws" } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Loading remote context failed", "/@context/@import");
}

TEST(JSONLD_expand_error, error_inside_remote_context) {
  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/invalid-term" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid term definition", "/@context");
}

TEST(JSONLD_expand_error, colliding_keywords) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "id": "@id" },
    "@id": "http://example.com/a",
    "id": "http://example.com/b"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Colliding keywords", "/id");
}

TEST(JSONLD_expand_error, invalid_id_value) {
  const auto input = sourcemeta::core::parse_json(R"({ "@id": true })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/@id");
}

TEST(JSONLD_expand_error, invalid_type_value) {
  const auto input = sourcemeta::core::parse_json(R"({ "@type": true })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type value", "/@type");
}

TEST(JSONLD_expand_error, invalid_value_object) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": {
      "@value": "x", "@type": "http://example.com/t", "@language": "en"
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object",
                             "/http:~1~1example.com~1p");
}

TEST(JSONLD_expand_error, invalid_language_tagged_string) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": "x", "@language": true }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid language-tagged string",
                             "/http:~1~1example.com~1p/@language");
}

TEST(JSONLD_expand_error, invalid_language_tagged_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": 1, "@language": "en" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid language-tagged value",
                             "/http:~1~1example.com~1p/@value");
}

TEST(JSONLD_expand_error, invalid_typed_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": "x", "@type": "_:b" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid typed value",
                             "/http:~1~1example.com~1p/@type");
}

TEST(JSONLD_expand_error, invalid_value_object_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": { "a": 1 } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object value",
                             "/http:~1~1example.com~1p/@value");
}

TEST(JSONLD_expand_error, invalid_set_or_list_object) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@list": [ "a" ], "@id": "http://example.com/x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object",
                             "/http:~1~1example.com~1p");
}

TEST(JSONLD_expand_error, invalid_index_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@index": true, "@value": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @index value",
                             "/http:~1~1example.com~1p/@index");
}

TEST(JSONLD_expand_error, invalid_reverse_value) {
  const auto input = sourcemeta::core::parse_json(R"({ "@reverse": "foo" })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @reverse value", "/@reverse");
}

TEST(JSONLD_expand_error, invalid_reverse_property_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": { "http://example.com/p": { "@value": "x" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value",
                             "/@reverse/http:~1~1example.com~1p");
}

TEST(JSONLD_expand_error, aliased_reverse_property_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "@reverse": { "p": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/@reverse/p");
}

TEST(JSONLD_expand_error, merged_aliased_reverse_property_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": "http://example.com/p",
      "b": "http://example.com/p"
    },
    "@reverse": {
      "a": { "@id": "http://example.com/ok" },
      "b": "x"
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/@reverse/b");
}

TEST(JSONLD_expand_error, aliased_reverse_keyword_property_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "rev": "@reverse" },
    "rev": { "http://example.com/p": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value",
                             "/rev/http:~1~1example.com~1p");
}

TEST(JSONLD_expand_error, keyword_inside_reverse_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": { "@id": "http://example.com/x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property map", "/@reverse/@id");
}

TEST(JSONLD_expand_error, invalid_included_value) {
  const auto input =
      sourcemeta::core::parse_json(R"({ "@included": { "@value": "x" } })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/@included");
}

TEST(JSONLD_expand_error, invalid_nest_value_expansion) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "nest": "@nest" },
    "nest": { "@value": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/nest");
}

TEST(JSONLD_expand_error, aliased_value_inside_nest) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "v": "@value" },
    "@nest": { "v": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/@nest");
}

TEST(JSONLD_expand_error, aliased_value_inside_aliased_nest) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "v": "@value", "data": "@nest" },
    "data": { "v": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/data");
}

TEST(JSONLD_expand_error, invalid_language_map_value_in_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "label": { "@id": "http://example.com/label", "@container": "@language" }
    },
    "label": { "en": [ "ok", 5 ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid language map value", "/label/en/1");
}

TEST(JSONLD_expand_error, invalid_id_inside_nest_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@nest": [ {}, { "@id": false } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/@nest/1/@id");
}

TEST(JSONLD_expand_error, invalid_id_inside_aliased_nest_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "data": "@nest" },
    "data": [ {}, { "@id": false } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/data/1/@id");
}

TEST(JSONLD_expand_error, list_of_lists) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@list": [ { "@list": [ "a" ] } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "List of lists", "/http:~1~1example.com~1p/@list/0");
}

TEST(JSONLD_expand_error, invalid_base_direction_in_value_object) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": { "@value": "v", "@direction": "up" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base direction", "/p/@direction");
}

TEST(JSONLD_expand_error, keyword_alias_dropped_inside_reverse_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "none": "@none" },
    "@reverse": { "none": "x" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property map", "/@reverse/none");
}

TEST(JSONLD_expand_error, colliding_type_in_json_ld_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "type": "@type" },
    "@type": "http://example.com/A",
    "type": "http://example.com/B"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Colliding keywords", "/type");
}

TEST(JSONLD_expand_error, invalid_language_tagged_string_without_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": { "@language": 42 }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid language-tagged string", "/p/@language");
}

TEST(JSONLD_expand_error, protected_in_term_definition_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@protected": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid term definition", "/@context/a/@protected");
}

TEST(JSONLD_expand_error, nest_in_term_definition_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@nest": "@nest" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid term definition", "/@context/a/@nest");
}

TEST(JSONLD_expand_error, prefix_on_compact_iri_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "ex": "http://example.com/",
      "ex:foo": { "@id": "http://example.com/foo", "@prefix": true }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition",
                             "/@context/ex:foo/@prefix");
}

TEST(JSONLD_expand_error, invalid_base_direction_in_term_definition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@direction": "up" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base direction",
                             "/@context/a/@direction");
}

TEST(JSONLD_expand_error, invalid_container_array_combination) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": { "@id": "http://example.com/a", "@container": [ "@id", "@language" ] }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/a/@container");
}

TEST(JSONLD_expand_error, invalid_container_set_with_multiple_keywords) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": {
        "@id": "http://example.com/a",
        "@container": [ "@set", "@id", "@language" ]
      }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/a/@container");
}

TEST(JSONLD_expand_error, explicit_json_literal_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": { "x": 1 }, "@type": "@json" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid value object value", "/http:~1~1example.com~1p/@value");
}

TEST(JSONLD_expand_error, explicit_json_literal_null_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": null, "@type": "@json" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid value object value", "/http:~1~1example.com~1p/@value");
}

TEST(JSONLD_expand_error, explicit_json_literal_scalar_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": 1, "@type": "@json" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid value object value", "/http:~1~1example.com~1p/@value");
}

TEST(JSONLD_expand_error, explicit_json_literal_aliased_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "val": "@value", "type": "@type" },
    "http://example.com/p": { "val": { "x": 1 }, "type": "@json" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid value object value", "/http:~1~1example.com~1p/val");
}

TEST(JSONLD_expand_error, relative_typed_value_datatype_without_base) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": "x", "@type": "relative" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid typed value",
                             "/http:~1~1example.com~1p/@type");
}

TEST(JSONLD_expand_error, list_object_with_type) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@list": [ "a" ], "@type": "http://example.com/T" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object",
                             "/http:~1~1example.com~1p");
}

TEST(JSONLD_expand_error, set_object_with_type) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@set": [ "a" ], "@type": "http://example.com/T" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object",
                             "/http:~1~1example.com~1p");
}

TEST(JSONLD_expand_error, list_object_with_set) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@list": [ "a" ], "@set": [ "b" ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object",
                             "/http:~1~1example.com~1p");
}

TEST(JSONLD_expand_error, aliased_list_object_with_type) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "list": "@list", "type": "@type" },
    "http://example.com/p": { "list": [ "a" ], "type": "http://example.com/T" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object",
                             "/http:~1~1example.com~1p");
}

TEST(JSONLD_expand_error, list_object_with_type_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@list": [ "a" ], "@type": "http://example.com/T" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid set or list object", "/http:~1~1example.com~1p");
}

TEST(JSONLD_expand_error, unknown_entry_in_term_definition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": "http://example.com/a", "@bogus": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition", "/@context/a/@bogus");
}

TEST(JSONLD_expand_error, type_keyword_container_id) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@type": { "@container": "@id" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Keyword redefinition", "/@context/@type");
}

TEST(JSONLD_expand_error, invalid_version_precedes_mode_conflict) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@version": 1.5 }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid @version value", "/@context/@version");
}

TEST(JSONLD_expand_error, relative_base_without_base) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ { "@base": null }, { "@base": "relative/path" } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base IRI", "/@context/1/@base");
}

TEST(JSONLD_expand_error, protected_null_term_redefinition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "term": { "@id": null, "@protected": true } },
      { "term": "http://example.com/x" }
    ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Protected term redefinition", "/@context/1/term");
}

TEST(JSONLD_expand_error, import_loading_failed) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@import": "https://example.com/unknown" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Loading remote context failed", "/@context/@import");
}

TEST(JSONLD_expand_error, duplicate_container_keyword) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": { "@id": "http://example.com/a", "@container": [ "@set", "@set" ] }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/a/@container/1");
}

TEST(JSONLD_expand_error, error_code_value_is_owned) {
  std::string code{"A custom error code longer than small string optimization"};
  const sourcemeta::core::JSONLDError error{code.c_str(),
                                            sourcemeta::core::Pointer{}};
  // Mutating the live buffer without freeing it distinguishes an owned copy
  // from a borrowed pointer deterministically
  std::ranges::fill(code, 'x');
  EXPECT_STREQ(error.what(),
               "A custom error code longer than small string optimization");
}

TEST(JSONLD_expand_error, resolver_jsonld_error_translated_for_direct_context) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    throw sourcemeta::core::JSONLDError("Invalid @id value",
                                        sourcemeta::core::Pointer{"foreign"});
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/context" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading remote context failed", "/@context");
}

TEST(JSONLD_expand_error, resolver_jsonld_error_translated_in_context_array) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    throw sourcemeta::core::JSONLDError("Invalid @id value",
                                        sourcemeta::core::Pointer{"foreign"});
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": [ {}, "https://example.com/context" ] })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading remote context failed", "/@context/1");
}

TEST(JSONLD_expand_error, resolver_jsonld_error_translated_for_import) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    throw sourcemeta::core::JSONLDError("Invalid @id value",
                                        sourcemeta::core::Pointer{"foreign"});
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@import": "https://example.com/context" } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading remote context failed", "/@context/@import");
}

TEST(JSONLD_expand_error, imported_invalid_language) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@import": "https://example.com/import-language" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid default language", "/@context/@import");
}

TEST(JSONLD_expand_error, imported_invalid_vocab) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@import": "https://example.com/import-vocab" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid vocab mapping", "/@context/@import");
}

TEST(JSONLD_expand_error, imported_invalid_vocab_in_context_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ { "@import": "https://example.com/import-vocab" } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid vocab mapping", "/@context/0/@import");
}

TEST(JSONLD_expand_error, local_override_of_imported_language) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@import": "https://example.com/import-language",
      "@language": 42
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", remote_resolver()),
      "Invalid default language", "/@context/@language");
}

TEST(JSONLD_expand_error, invalid_local_context_under_scoped_property) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@context": { "@propagate": false, "a": "http://example.com/a" }
      }
    },
    "p": { "@context": false, "a": "v" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid local context", "/p/@context");
}

TEST(JSONLD_expand_error, reverse_term_list_container) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@reverse": "http://example.org/p", "@container": "@list" }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property",
                             "/@context/p/@container");
}

TEST(JSONLD_expand_error, null_id_term_with_invalid_type) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": null, "@type": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type mapping", "/@context/a/@type");
}

TEST(JSONLD_expand_error, null_id_term_with_reverse) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": null, "@reverse": "http://example.com/r" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property",
                             "/@context/a/@reverse");
}

TEST(JSONLD_expand_error, local_scoped_context_base_is_validated) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "http://example.com/p", "@context": { "@base": 42 } }
    },
    "http://example.com/q": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid scoped context", "/@context/p/@context");
}

TEST(JSONLD_expand_error, reverse_map_error_uses_its_local_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": {
      "@context": { "p": "https://example.com/p" },
      "p": { "@value": 42 }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/@reverse/p");
}

TEST(JSONLD_expand_error, reverse_map_error_without_local_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": { "https://example.com/p": { "@value": 42 } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value",
                             "/@reverse/https:~1~1example.com~1p");
}

TEST(JSONLD_expand_error, protected_nest_redefinition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      {
        "p": {
          "@id": "https://example.com/p",
          "@protected": true,
          "@nest": "meta"
        }
      },
      { "p": { "@id": "https://example.com/p", "@nest": "other" } }
    ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Protected term redefinition", "/@context/1/p");
}

TEST(JSONLD_expand_error, id_map_array_item_error_location) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "https://example.com/p", "@container": "@id" }
    },
    "p": { "urn:n": [ {}, { "@id": false } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/p/urn:n/1/@id");
}

TEST(JSONLD_expand_error, id_map_object_value_error_location) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "https://example.com/p", "@container": "@id" }
    },
    "p": { "urn:n": { "@id": false } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/p/urn:n/@id");
}

TEST(JSONLD_expand_error, nearby_version_number) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@version": 1.1000000005 },
    "https://example.com/p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @version value", "/@context/@version");
}

TEST(JSONLD_expand_error, nearby_version_number_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@version": 1.1000000005 },
    "https://example.com/p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid @version value", "/@context/@version");
}

TEST(JSONLD_expand_error, invalid_nest_member_with_alias) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "n": "@nest" },
    "n": [ {}, "bad" ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/n/1");
}

TEST(JSONLD_expand_error, invalid_nest_member_literal) {
  const auto input = sourcemeta::core::parse_json(R"({
    "https://example.com/p": "v",
    "@nest": [ {}, "bad" ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/@nest/1");
}

TEST(JSONLD_expand_error, nest_member_error_keeps_item_location) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "n": "@nest" },
    "n": [ {}, { "@id": false } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/n/1/@id");
}

TEST(JSONLD_expand_error, malformed_expansion_context_base) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": "v"
  })");
  const auto context = sourcemeta::core::parse_json(R"({ "@base": 42 })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, context),
                             "Invalid base IRI", "");
}

TEST(JSONLD_expand_error, malformed_context_reference) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/%zz" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading document failed", "/@context");
  EXPECT_EQ(invocations, 0);
}

TEST(JSONLD_expand_error, malformed_context_reference_with_base) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": "https://example.com/%zz" })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "https://example.com/", resolver),
      "Loading document failed", "/@context");
  EXPECT_EQ(invocations, 0);
}

TEST(JSONLD_expand_error, malformed_context_reference_in_array) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": [ {}, "https://example.com/%zz" ] })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading document failed", "/@context/1");
  EXPECT_EQ(invocations, 0);
}

TEST(JSONLD_expand_error, malformed_import_reference) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(
      R"({ "@context": { "@import": "https://example.com/%zz" } })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading remote context failed", "/@context/@import");
  EXPECT_EQ(invocations, 0);
}

TEST(JSONLD_expand_error, context_direction_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@direction": "rtl", "p": "https://example.com/p" },
    "p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid context entry", "/@context/@direction");
}

TEST(JSONLD_expand_error, nest_in_reverse_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": { "@nest": {} }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property map", "/@reverse/@nest");
}

TEST(JSONLD_expand_error, aliased_nest_in_reverse_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "n": "@nest" },
    "@reverse": { "n": { "https://example.com/p": "v" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property map", "/@reverse/n");
}

TEST(JSONLD_expand_error, deferred_external_context_error_at_root) {
  const auto input = sourcemeta::core::parse_json(R"({ "@type": "T" })");
  const auto context = sourcemeta::core::parse_json(R"({
    "p": { "@id": "https://example.com/p", "@protected": true },
    "T": {
      "@id": "https://example.com/T",
      "@context": { "p": "https://example.com/other" }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, context),
                             "Protected term redefinition", "");
}

TEST(JSONLD_expand_error, empty_type_keyword_definition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@type": {} },
    "@type": "urn:T"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Keyword redefinition", "/@context/@type");
}

TEST(JSONLD_expand_error, null_nest_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "https://example.com/p": "v",
    "@nest": null
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/@nest");
}

TEST(JSONLD_expand_error, null_aliased_nest_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "n": "@nest" },
    "https://example.com/p": "v",
    "n": null
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @nest value", "/n");
}

TEST(JSONLD_expand_error, first_invalid_term_is_lexicographic) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "z": false, "a": false }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition", "/@context/a");

  const auto reversed = sourcemeta::core::parse_json(R"({
    "@context": { "a": false, "z": false }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(reversed),
                             "Invalid term definition", "/@context/a");
}

TEST(JSONLD_expand_error, first_invalid_mapping_is_lexicographic) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "z": { "@id": false }, "a": { "@id": false } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid IRI mapping", "/@context/a/@id");

  const auto reversed = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": false }, "z": { "@id": false } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(reversed),
                             "Invalid IRI mapping", "/@context/a/@id");
}

TEST(JSONLD_expand_error, graph_container_with_both_map_kinds) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "https://example.com/p",
        "@container": [ "@graph", "@id", "@index" ]
      }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/p/@container");
}

TEST(JSONLD_expand_error, graph_container_with_both_map_kinds_and_set) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "https://example.com/p",
        "@container": [ "@graph", "@id", "@index", "@set" ]
      }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/p/@container");
}

TEST(JSONLD_expand_error, protected_context_nullification_mid_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ { "@protected": true, "a": "urn:a" }, null ],
    "urn:p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid context nullification", "/@context/1");
}

TEST(JSONLD_expand_error, invalid_value_object_in_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:foo": [ { "@value": "a" }, { "@value": "b", "@id": "urn:x" } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object", "/urn:foo/1");
}

TEST(JSONLD_expand_error, invalid_member_in_array_object) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:foo": [ {}, { "@id": false } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @id value", "/urn:foo/1/@id");
}

TEST(JSONLD_expand_error, invalid_context_protected_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@protected": "yes", "p": "urn:p" },
    "p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @protected value",
                             "/@context/@protected");
}

TEST(JSONLD_expand_error, invalid_context_protected_value_in_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ {}, { "@protected": 1, "p": "urn:p" } ],
    "p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @protected value",
                             "/@context/1/@protected");
}

TEST(JSONLD_expand_error, null_context_protected_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@protected": null, "p": "urn:p" },
    "p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @protected value",
                             "/@context/@protected");
}

TEST(JSONLD_expand_error, deferred_local_scoped_error_keeps_term_pointer) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@protected": true,
      "a": "http://example.com/a",
      "Type": {
        "@id": "http://example.com/Type",
        "@context": { "a": "http://example.com/other" }
      }
    },
    "@type": "Type",
    "a": "foo"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Protected term redefinition",
                             "/@context/Type/@context/a");
}

TEST(JSONLD_expand_error, imported_dependency_error_points_to_import) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/import-dependency") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "z": { "@id": 12 } } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@import": "https://example.com/import-dependency",
      "a": "z"
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Invalid IRI mapping", "/@context/@import");
}

TEST(JSONLD_expand_error, index_map_member_keeps_original_position) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "urn:p",
        "@container": "@index",
        "@index": "urn:i"
      }
    },
    "p": { "x": [ null, { "@value": "bad" } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object", "/p/x/1");
}

TEST(JSONLD_expand_error, index_map_single_member_keeps_entry_location) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "urn:p",
        "@container": "@index",
        "@index": "urn:i"
      }
    },
    "p": { "x": { "@value": "bad" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object", "/p/x");
}

TEST(JSONLD_expand_error, included_array_member_keeps_original_position) {
  const auto input = sourcemeta::core::parse_json(R"({
    "https://example.com/p": "v",
    "@included": [ { "@id": "urn:n" }, { "@value": "x" } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/@included/1");
}

TEST(JSONLD_expand_error, aliased_included_array_member_position) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "inc": "@included" },
    "https://example.com/p": "v",
    "inc": [ { "@id": "urn:n" }, { "@value": "x" } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/inc/1");
}

TEST(JSONLD_expand_error, nested_external_context_error_at_root) {
  const auto input = sourcemeta::core::parse_json(R"({
    "p": { "@type": "T" }
  })");
  const auto context = sourcemeta::core::parse_json(R"({
    "a": { "@id": "urn:a", "@protected": true },
    "p": {
      "@id": "urn:p",
      "@context": {
        "T": { "@id": "urn:T", "@context": { "a": "urn:other" } }
      }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, context),
                             "Protected term redefinition", "");
}

TEST(JSONLD_expand_error, nested_external_protected_scope_error_at_root) {
  const auto input = sourcemeta::core::parse_json(R"({
    "p": { "@type": "T" }
  })");
  const auto context = sourcemeta::core::parse_json(R"({
    "p": {
      "@id": "urn:p",
      "@context": {
        "@protected": true,
        "a": "urn:a",
        "T": { "@id": "urn:T", "@context": { "a": "urn:other" } }
      }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, context),
                             "Protected term redefinition", "");
}

TEST(JSONLD_expand_error, invalid_type_member_keeps_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@type": [ "urn:ok", 5 ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type value", "/@type/1");
}

TEST(JSONLD_expand_error, invalid_aliased_type_member_keeps_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "t": "@type" },
    "t": [ "urn:ok", 5 ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type value", "/t/1");
}

TEST(JSONLD_expand_error, invalid_type_member_keeps_index_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@type": [ "urn:ok", 5 ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "Invalid type value", "/@type/1");
}

TEST(JSONLD_expand_error, invalid_reverse_term_member_keeps_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "r": { "@reverse": "urn:p" } },
    "r": [ { "@id": "urn:a" }, { "@value": 1 } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/r/1");
}

TEST(JSONLD_expand_error, invalid_reverse_map_member_keeps_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": { "urn:p": [ { "@id": "urn:a" }, { "@value": 1 } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value",
                             "/@reverse/urn:p/1");
}

TEST(JSONLD_expand_error, included_nested_array_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "https://example.com/p": "v",
    "@included": [ { "@id": "urn:ok" }, [ { "@id": "urn:n" }, { "@value": "x" } ] ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/@included/1/1");
}

TEST(JSONLD_expand_error, included_set_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "https://example.com/p": "v",
    "@included": [
      { "@id": "urn:ok" },
      { "@set": [ { "@id": "urn:n" }, { "@value": "x" } ] }
    ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/@included/1/@set/1");
}

TEST(JSONLD_expand_error, equivalent_protected_redefinition_retains_origin) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "a": { "@id": "urn:a", "@protected": true },
      "T": {
        "@id": "urn:T",
        "@protected": true,
        "@context": { "a": "urn:other" }
      }
    },
    "@type": "T",
    "a": "x"
  })");
  const auto context = sourcemeta::core::parse_json(R"({
    "a": { "@id": "urn:a", "@protected": true },
    "T": {
      "@id": "urn:T",
      "@protected": true,
      "@context": { "a": "urn:other" }
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, context),
                             "Protected term redefinition", "");
}

TEST(JSONLD_expand_error, reverse_definition_rejects_unknown_entries) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@reverse": "urn:p", "@bogus": true } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition", "/@context/p/@bogus");
}

TEST(JSONLD_expand_error, index_map_nested_array_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "urn:p", "@container": "@index", "@index": "urn:i" }
    },
    "p": { "x": [ null, [ null, { "@value": "bad" } ] ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object", "/p/x/1/1");
}

TEST(JSONLD_expand_error, index_map_set_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "urn:p", "@container": "@index", "@index": "urn:i" }
    },
    "p": { "x": [ null, { "@set": [ null, { "@value": "bad" } ] } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object", "/p/x/1/@set/1");
}

TEST(JSONLD_expand_error, index_map_direct_set_value_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "urn:p", "@container": "@index", "@index": "urn:i" }
    },
    "p": { "x": { "@set": [ null, { "@value": "bad" } ] } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid value object", "/p/x/@set/1");
}

TEST(JSONLD_expand_error, reverse_nested_array_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "r": { "@reverse": "urn:p" } },
    "r": [ null, [ { "@value": 1 } ] ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/r/1/0");
}

TEST(JSONLD_expand_error, reverse_set_value_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "r": { "@reverse": "urn:p" } },
    "r": { "@set": [ null, { "@value": "x" } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/r/@set/1");
}

TEST(JSONLD_expand_error, reverse_map_nested_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@reverse": { "urn:p": [ null, [ { "@value": 1 } ] ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value",
                             "/@reverse/urn:p/1/0");
}

TEST(JSONLD_expand_error, legacy_list_container_member_keeps_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "urn:p", "@container": "@list" } },
    "p": [ null, { "@list": [ "x" ] } ]
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "List of lists", "/p/1");
}

TEST(JSONLD_expand_error, protected_type_empty_redefinition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ { "@type": { "@protected": true } }, { "@type": {} } ],
    "@type": "urn:T"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Keyword redefinition", "/@context/1/@type");
}

TEST(JSONLD_expand_error, imported_term_local_dependency_keeps_location) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/import-local-dependency") {
      return sourcemeta::core::parse_json(R"({ "@context": { "a": "z" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@import": "https://example.com/import-local-dependency",
      "z": { "@id": 42 }
    },
    "a": "x"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Invalid IRI mapping", "/@context/z/@id");
}

TEST(JSONLD_expand_error, blank_node_remote_context_rejected) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return sourcemeta::core::parse_json(R"({ "@context": {} })");
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": "_:ctx"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading document failed", "/@context");
  EXPECT_EQ(invocations, 0);
}

TEST(JSONLD_expand_error, blank_node_import_rejected) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return sourcemeta::core::parse_json(R"({ "@context": {} })");
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@import": "_:ctx" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Loading remote context failed", "/@context/@import");
  EXPECT_EQ(invocations, 0);
}

TEST(JSONLD_expand_error, included_set_alias_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@included": {
      "@context": { "s": "@set" },
      "s": [ null, { "@value": "x" } ]
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/@included/s/1");
}

TEST(JSONLD_expand_error, included_set_alias_member_after_valid_node) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@included": {
      "@context": { "s": "@set" },
      "s": [ { "@id": "urn:n" }, { "@value": "x" } ]
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid @included value", "/@included/s/1");
}

TEST(JSONLD_expand_error, invalid_container_member_keeps_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "urn:p", "@container": [ "@set", false ] } },
    "urn:q": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/p/@container/1");
}

TEST(JSONLD_expand_error, unresolved_property_valued_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "urn:p", "@container": "@index", "@index": "relative" }
    },
    "p": { "x": {} }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition", "/@context/p/@index");
}

TEST(JSONLD_expand_error, ignored_type_alias_preserves_provenance) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@vocab": "urn:", "a": "@type", "z": "@type" },
    "urn:p": { "@value": 1, "a": "_:blank", "z": "@ignore" }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid typed value", "/urn:p/a");
}

TEST(JSONLD_expand_error, relative_base_without_caller_base) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@base": "relative/path" },
    "urn:p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base IRI", "/@context/@base");
}

TEST(JSONLD_expand_error, relative_base_in_second_context_entry) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ null, { "@base": "relative/path" } ],
    "urn:p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid base IRI", "/@context/1/@base");
}

TEST(JSONLD_expand_error, relative_base_in_expand_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": "v"
  })");
  const auto context = sourcemeta::core::parse_json(R"({
    "@base": "relative/path"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input, context),
                             "Invalid base IRI", "");
}

TEST(JSONLD_expand_error, explicit_list_set_member_keeps_indices) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": { "@list": [ { "@set": [ null, { "@list": [ "x" ] } ] } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", {},
                                      sourcemeta::core::JSONLDVersion::V1_0),
      "List of lists", "/urn:p/@list/0/@set/1");
}

TEST(JSONLD_expand_error, unknown_term_entries_report_lexicographically) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "urn:p", "z": false, "a": false } },
    "urn:q": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition", "/@context/p/a");
}

TEST(JSONLD_expand_error, unknown_term_entries_lexicographic_reversed) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "urn:p", "a": false, "z": false } },
    "urn:q": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid term definition", "/@context/p/a");
}

TEST(JSONLD_expand_error, imported_name_keeps_nested_scope_validation) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/import-collision") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "urn:imported" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@import": "https://example.com/import-collision",
      "T": {
        "@id": "urn:T",
        "@context": {
          "p": { "@id": "urn:local", "@context": { "@base": 42 } }
        }
      }
    },
    "urn:q": "x"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(
      sourcemeta::core::jsonld_expand(input, "", resolver),
      "Invalid scoped context", "/@context/T/@context");
}

TEST(JSONLD_expand_error, nested_scope_validation_without_import) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "T": {
        "@id": "urn:T",
        "@context": {
          "p": { "@id": "urn:local", "@context": { "@base": 42 } }
        }
      }
    },
    "urn:q": "x"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid scoped context", "/@context/T/@context");
}

TEST(JSONLD_expand_error, nested_shape_pointer) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "n": "@nest" },
    "urn:p": { "n": { "@list": [ "x" ], "@set": [ "y" ] } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object", "/urn:p/n");
}

TEST(JSONLD_expand_error, nested_shape_pointer_array_member) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "n": "@nest" },
    "urn:p": {
      "n": [
        { "urn:q": "v" },
        { "@list": [ "x" ], "@set": [ "y" ] }
      ]
    }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object", "/urn:p/n/1");
}

TEST(JSONLD_expand_error, invalid_array_datatype_on_value_object) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": { "@value": "x", "@type": [ "@foo" ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid typed value", "/urn:p/@type");
}

TEST(JSONLD_expand_error, invalid_array_datatype_on_aliased_type) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "t": "@type" },
    "urn:p": { "@value": "x", "t": [ "@foo" ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid typed value", "/urn:p/t");
}

TEST(JSONLD_expand_error, empty_container_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "urn:p", "@container": [] } },
    "p": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid container mapping",
                             "/@context/p/@container");
}

TEST(JSONLD_expand_error, invalid_type_with_keyword_form_id) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "@foo", "@type": false } },
    "urn:q": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type mapping", "/@context/p/@type");
}

TEST(JSONLD_expand_error, invalid_type_with_keyword_form_reverse) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@reverse": "@future", "@type": false } },
    "urn:q": "v"
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid type mapping", "/@context/p/@type");
}

TEST(JSONLD_expand_error, null_set_inside_nest_keeps_location) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": { "@nest": { "@set": null, "@type": "urn:T" } }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object", "/urn:p/@nest");
}

TEST(JSONLD_expand_error, null_set_inside_nest_array_keeps_location) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": { "@nest": [ { "@set": null, "@type": "urn:T" } ] }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid set or list object", "/urn:p/@nest/0");
}

TEST(JSONLD_expand_error, reverse_alias_errors_report_lexicographically) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": "urn:p", "z": "urn:p" },
    "@reverse": { "z": 1, "a": 2 }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/@reverse/a");
}

TEST(JSONLD_expand_error, reverse_alias_lexicographic_reversed) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": "urn:p", "z": "urn:p" },
    "@reverse": { "a": 2, "z": 1 }
  })");

  EXPECT_JSONLD_EXPAND_ERROR(sourcemeta::core::jsonld_expand(input),
                             "Invalid reverse property value", "/@reverse/a");
}
