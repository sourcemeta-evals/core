#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>

#include <algorithm> // std::sort
#include <optional>  // std::optional, std::nullopt

TEST(JSONLD_expand, empty_object) {
  const auto input = sourcemeta::core::parse_json("{}");
  const auto expected = sourcemeta::core::parse_json("[]");
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, absolute_iri_property_with_string_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/foo": "bar"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/foo": [ { "@value": "bar" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, node_with_id_and_property) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@id": "http://example.com/a",
    "http://example.com/foo": "bar"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/a",
      "http://example.com/foo": [ { "@value": "bar" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, type_is_made_an_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@id": "http://example.com/a",
    "@type": "http://example.com/T",
    "http://example.com/foo": "bar"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/a",
      "@type": [ "http://example.com/T" ],
      "http://example.com/foo": [ { "@value": "bar" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, multiple_values_preserved) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/foo": [ "a", "b" ]
  })");

  auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/foo": [ { "@value": "a" }, { "@value": "b" } ]
    }
  ])");

  auto result = sourcemeta::core::jsonld_expand(input);

  // Expanded values of an ordinary property are an unordered set, so the
  // comparison must not depend on emission order
  auto &result_values = result.at(0).at("http://example.com/foo");
  std::sort(result_values.as_array().begin(), result_values.as_array().end());
  auto &expected_values = expected.at(0).at("http://example.com/foo");
  std::sort(expected_values.as_array().begin(),
            expected_values.as_array().end());

  EXPECT_EQ(result, expected);
}

TEST(JSONLD_expand, numeric_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/foo": 1
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/foo": [ { "@value": 1 } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, boolean_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/foo": true
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/foo": [ { "@value": true } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, undefined_term_without_context_is_dropped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "foo": "bar"
  })");

  const auto expected = sourcemeta::core::parse_json("[]");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, free_floating_list_is_dropped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@list": [ "foo" ], "@id": "http://example.com/bar"
  })");

  const auto expected = sourcemeta::core::parse_json("[]");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, term_maps_to_iri) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "name": "http://example.com/name" },
    "name": "John"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/name": [ { "@value": "John" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, vocabulary_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@vocab": "http://example.com/" },
    "name": "John"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/name": [ { "@value": "John" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, compact_iri_via_prefix) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "ex": "http://example.com/" },
    "ex:name": "John"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/name": [ { "@value": "John" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, type_coercion_to_id) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "knows": { "@id": "http://example.com/knows", "@type": "@id" }
    },
    "knows": "http://example.com/jane"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/knows": [ { "@id": "http://example.com/jane" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, type_coercion_to_datatype) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "born": {
        "@id": "http://example.com/born",
        "@type": "http://www.w3.org/2001/XMLSchema#date"
      }
    },
    "born": "1990-01-01"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/born": [
        {
          "@value": "1990-01-01",
          "@type": "http://www.w3.org/2001/XMLSchema#date"
        }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, default_language) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@language": "en", "name": "http://example.com/name" },
    "name": "John"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/name": [ { "@value": "John", "@language": "en" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, list_keyword) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/foo": { "@list": [ "a", "b" ] }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/foo": [
        { "@list": [ { "@value": "a" }, { "@value": "b" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, direction_dropped_in_json_ld_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": { "@value": "v", "@direction": "rtl" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, "", {}, sourcemeta::core::JSONLDVersion::V1_0),
            expected);
}

TEST(JSONLD_expand, included_dropped_in_json_ld_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": "v",
    "@included": { "@id": "http://example.com/other" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, "", {}, sourcemeta::core::JSONLDVersion::V1_0),
            expected);
}

TEST(JSONLD_expand, included_identifier_only_node) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": "v",
    "@included": { "@id": "http://example.com/n" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [ { "@value": "v" } ],
      "@included": [ { "@id": "http://example.com/n" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, included_identifier_only_node_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": "v",
    "@included": [ { "@id": "http://example.com/n" } ]
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [ { "@value": "v" } ],
      "@included": [ { "@id": "http://example.com/n" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, decimal_version_value) {
  auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "http://example.com/p" },
    "p": "v"
  })");
  input.at("@context")
      .assign("@version",
              sourcemeta::core::JSON{sourcemeta::core::Decimal{"1.1"}});

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, relative_typed_value_datatype_resolved_against_base) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@value": "x", "@type": "relative" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@value": "x", "@type": "https://example.com/relative" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "https://example.com/"),
            expected);
}

TEST(JSONLD_expand, null_id_map_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@id" }
    },
    "p": {
      "https://example.com/a": null,
      "https://example.com/b": {}
    }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [ { "@id": "https://example.com/b" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, null_type_map_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@type" }
    },
    "p": { "http://example.com/T": null }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, null_graph_index_map_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@container": [ "@graph", "@index" ]
      }
    },
    "p": { "i": null }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, protected_redefinition_with_reordered_container) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      {
        "@protected": true,
        "p": {
          "@id": "http://example.com/p",
          "@container": [ "@set", "@index" ]
        }
      },
      {
        "p": {
          "@id": "http://example.com/p",
          "@container": [ "@index", "@set" ]
        }
      }
    ],
    "p": { "a": "v" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [ { "@value": "v", "@index": "a" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, null_index_map_value) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "http://example.com/p", "@container": "@index" }
    },
    "p": { "a": null }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, nest_term_whose_scoped_context_redefines_itself) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "nest": {
        "@id": "@nest",
        "@context": { "nest": { "@id": "http://example.com/nest" } }
      }
    },
    "nest": { "http://example.com/foo": "bar" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/foo": [ { "@value": "bar" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, json_typed_value_in_list_container) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "e": {
        "@id": "http://example.com/e",
        "@type": "@json",
        "@container": "@list"
      }
    },
    "e": 42
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/e": [
        { "@list": [ { "@value": 42, "@type": "@json" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, id_typed_keyword_form_value_expands_to_null) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "http://example.com/p", "@type": "@id" } },
    "p": "@foo"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@id": null } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, non_expandable_type_value_is_omitted) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@id": "http://example.com/n",
    "@type": "@foo",
    "http://example.com/p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://example.com/n",
      "http://example.com/p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, graph_value_expanding_to_null_yields_no_element) {
  const auto input = sourcemeta::core::parse_json(R"({ "@graph": "scalar" })");
  const auto expected = sourcemeta::core::parse_json("[]");
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, relative_context_resolved_against_base) {
  sourcemeta::core::JSON::String resolved_identifier;
  const sourcemeta::core::JSONLDResolver resolver =
      [&resolved_identifier](
          const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    resolved_identifier = identifier;
    if (identifier == "https://example.com/dir/context.jsonld") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "http://example.com/p" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": "context.jsonld",
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, "https://example.com/dir/document.jsonld", resolver),
            expected);
  EXPECT_EQ(resolved_identifier, "https://example.com/dir/context.jsonld");
}

TEST(JSONLD_expand, scoped_none_alias_in_language_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@container": "@language",
        "@context": { "none": "@none", "@direction": "rtl" }
      }
    },
    "p": { "en": "y", "none": "x" },
    "none": "z"
  })");

  auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@value": "y", "@language": "en", "@direction": "rtl" },
        { "@value": "x", "@direction": "rtl" }
      ]
    }
  ])");

  auto result = sourcemeta::core::jsonld_expand(input);

  // Expanded values of an ordinary property are an unordered set, so the
  // comparison must not depend on emission order
  auto &result_values = result.at(0).at("http://example.com/p");
  std::sort(result_values.as_array().begin(), result_values.as_array().end());
  auto &expected_values = expected.at(0).at("http://example.com/p");
  std::sort(expected_values.as_array().begin(),
            expected_values.as_array().end());

  EXPECT_EQ(result, expected);
}

TEST(JSONLD_expand, list_object_with_index) {
  const auto input = sourcemeta::core::parse_json(R"({
    "http://example.com/p": { "@list": [ "a" ], "@index": "i" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@list": [ { "@value": "a" } ], "@index": "i" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, unicode_iri_term_mapping) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "café": "https://example.com/café" },
    "café": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "https://example.com/café": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, unicode_relative_id_resolved_against_base) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@id": "café",
    "http://example.com/p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://example.com/dir/café",
      "http://example.com/p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "https://example.com/dir/"),
            expected);
}

TEST(JSONLD_expand, unicode_remote_context_reference) {
  sourcemeta::core::JSON::String resolved_identifier;
  const sourcemeta::core::JSONLDResolver resolver =
      [&resolved_identifier](
          const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    resolved_identifier = identifier;
    if (identifier == "https://example.com/ctx-café.jsonld") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "http://example.com/p" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": "ctx-café.jsonld",
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(
      sourcemeta::core::jsonld_expand(input, "https://example.com/", resolver),
      expected);
  EXPECT_EQ(resolved_identifier, "https://example.com/ctx-café.jsonld");
}

TEST(JSONLD_expand, remote_scoped_context_base_is_ignored) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/remote-scoped") {
      return sourcemeta::core::parse_json(R"({
        "@context": {
          "p": {
            "@id": "http://example.com/p",
            "@context": { "@base": "https://remote.example/" }
          }
        }
      })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": "https://example.com/remote-scoped",
    "p": { "@id": "child" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@id": "https://local.example/dir/child" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "https://local.example/dir/",
                                            resolver),
            expected);
}

TEST(JSONLD_expand, imported_scoped_context_base_is_ignored) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/import-scoped") {
      return sourcemeta::core::parse_json(R"({
        "@context": {
          "p": {
            "@id": "http://example.com/p",
            "@context": { "@base": "https://remote.example/" }
          }
        }
      })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@import": "https://example.com/import-scoped" },
    "p": { "@id": "child" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@id": "https://local.example/dir/child" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "https://local.example/dir/",
                                            resolver),
            expected);
}

TEST(JSONLD_expand, local_scoped_context_base_applies) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@context": { "@base": "https://scoped.example/" }
      }
    },
    "p": { "@id": "child" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@id": "https://scoped.example/child" }
      ]
    }
  ])");

  EXPECT_EQ(
      sourcemeta::core::jsonld_expand(input, "https://local.example/dir/"),
      expected);
}

TEST(JSONLD_expand, imported_base_is_ignored) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/import-base") {
      return sourcemeta::core::parse_json(R"({
        "@context": {
          "@base": "https://remote.example/",
          "q": "http://example.com/q"
        }
      })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@import": "https://example.com/import-base",
      "p": "http://example.com/p"
    },
    "@id": "relative-node",
    "p": "v",
    "q": "w"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://local.example/dir/relative-node",
      "http://example.com/p": [ { "@value": "v" } ],
      "http://example.com/q": [ { "@value": "w" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "https://local.example/dir/",
                                            resolver),
            expected);
}

TEST(JSONLD_expand, repeated_sibling_remote_references) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/sibling") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "http://example.com/p" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      "https://example.com/sibling",
      "https://example.com/sibling"
    ],
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), expected);
}

TEST(JSONLD_expand, local_base_after_remote_context) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/sibling") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "http://example.com/p" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      "https://example.com/sibling",
      { "@base": "https://local.example/" }
    ],
    "@id": "relative-node",
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://local.example/relative-node",
      "http://example.com/p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), expected);
}

TEST(JSONLD_expand, base_in_remote_context_is_ignored) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/remote-base") {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "@base": "http://remote.example/" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": "https://example.com/remote-base",
    "@id": "relative-node",
    "http://example.com/p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "http://doc.example/relative-node",
      "http://example.com/p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(
      sourcemeta::core::jsonld_expand(input, "http://doc.example/", resolver),
      expected);
}

TEST(JSONLD_expand, language_map_direction_uses_property_scoped_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@container": "@language",
        "@context": { "@direction": "rtl" }
      }
    },
    "p": { "en": "hello" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@value": "hello", "@language": "en", "@direction": "rtl" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}
