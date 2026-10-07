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

TEST(JSONLD_expand, none_alias_in_language_map) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "none": "@none",
      "@direction": "rtl",
      "p": { "@id": "http://example.com/p", "@container": "@language" }
    },
    "p": { "en": "y", "none": "x" }
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

TEST(JSONLD_expand, language_map_direction_uses_term_definition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@direction": "ltr",
      "p": {
        "@id": "http://example.com/p",
        "@container": "@language",
        "@direction": "rtl"
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

TEST(JSONLD_expand, scoped_alias_invisible_to_language_map_keys) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@container": "@language",
        "@context": { "none": "@none" }
      }
    },
    "p": { "none": "x" }
  })");

  // Language map keys read the element's own active context, so an alias
  // defined only in the property-scoped context does not strip @language
  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@value": "x", "@language": "none" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, language_map_direction_uses_property_scoped_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@direction": "ltr",
      "p": {
        "@id": "http://example.com/p",
        "@container": "@language",
        "@context": { "@direction": "rtl" }
      },
      "q": { "@id": "http://example.com/q", "@container": "@language" }
    },
    "p": { "en": "hello" },
    "q": { "en": "world" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@value": "hello", "@language": "en", "@direction": "rtl" }
      ],
      "http://example.com/q": [
        { "@value": "world", "@language": "en", "@direction": "ltr" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, scoped_null_direction_clears_language_map_direction) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@direction": "ltr",
      "p": {
        "@id": "http://example.com/p",
        "@container": "@language",
        "@context": { "@direction": null }
      }
    },
    "p": { "en": "hello" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "@value": "hello", "@language": "en" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, language_map_term_direction_wins_over_scoped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@container": "@language",
        "@direction": "rtl",
        "@context": { "@direction": "ltr" }
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

TEST(JSONLD_expand, context_direction_applies_in_1_1) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@direction": "rtl", "p": "https://example.com/p" },
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        { "@value": "v", "@direction": "rtl" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, reverse_keyword_form_value_is_ignored) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@reverse": "@foo" } },
    "http://example.com/q": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/q": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, reverse_term_null_container) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@reverse": "http://example.org/p", "@container": null }
    },
    "p": { "@id": "http://example.org/o" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@reverse": {
        "http://example.org/p": [ { "@id": "http://example.org/o" } ]
      }
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, reverse_term_null_container_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@reverse": "http://example.org/p", "@container": null }
    },
    "p": { "@id": "http://example.org/o" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@reverse": {
        "http://example.org/p": [ { "@id": "http://example.org/o" } ]
      }
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, "", {}, sourcemeta::core::JSONLDVersion::V1_0),
            expected);
}

TEST(JSONLD_expand, null_type_scoped_context_restores_outer_context) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@vocab": "https://example.com/",
      "T": { "@id": "https://example.com/T", "@context": null }
    },
    "@type": "T",
    "q": "drop",
    "https://example.com/child": { "p": "keep" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@type": [ "https://example.com/T" ],
      "https://example.com/child": [
        { "https://example.com/p": [ { "@value": "keep" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, null_id_term_drops_property) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "a": { "@id": null } },
    "a": "x",
    "https://example.com/p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "https://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, imported_scoped_malformed_base_is_ignored) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/import-scoped-base") {
      return sourcemeta::core::parse_json(R"({
        "@context": {
          "p": {
            "@id": "http://example.com/p",
            "@context": { "@base": 42, "q": "http://example.com/q" }
          }
        }
      })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@import": "https://example.com/import-scoped-base" },
    "p": { "@id": "child", "q": "v" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        {
          "@id": "https://caller.example/dir/child",
          "http://example.com/q": [ { "@value": "v" } ]
        }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, "https://caller.example/dir/", resolver),
            expected);
}

TEST(JSONLD_expand, recursive_scoped_context_is_valid) {
  const sourcemeta::core::JSONLDResolver resolver =
      [](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/self-scoped") {
      return sourcemeta::core::parse_json(R"({
        "@context": {
          "p": {
            "@id": "http://example.com/p",
            "@context": "https://example.com/self-scoped"
          }
        }
      })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": "https://example.com/self-scoped",
    "p": { "http://example.com/q": "v" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "http://example.com/q": [ { "@value": "v" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), expected);
}

TEST(JSONLD_expand, nonpropagating_scoped_context_local_override) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "http://example.com/p",
        "@context": { "a": "http://example.com/scoped-a", "@propagate": false }
      }
    },
    "p": { "@context": { "a": "http://example.com/local-a" }, "a": "v" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "http://example.com/local-a": [ { "@value": "v" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, nonpropagating_scoped_context_array_values) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@vocab": "http://example.com/",
      "p": {
        "@id": "http://example.com/p",
        "@context": { "q": "http://example.com/scoped-q", "@propagate": false }
      }
    },
    "p": [ { "q": { "q": "deep" } } ]
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        {
          "http://example.com/scoped-q": [
            { "http://example.com/q": [ { "@value": "deep" } ] }
          ]
        }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, expansion_context_base_applies) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@id": "child",
    "http://example.com/p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://ctx.example/dir/child",
      "http://example.com/p": [ { "@value": "v" } ]
    }
  ])");

  const auto bare = sourcemeta::core::parse_json(
      R"({ "@base": "https://ctx.example/dir/" })");
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, bare), expected);

  const auto wrapped = sourcemeta::core::parse_json(
      R"({ "@context": { "@base": "https://ctx.example/dir/" } })");
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, wrapped), expected);

  const auto array = sourcemeta::core::parse_json(
      R"([ { "@base": "https://ctx.example/dir/" } ])");
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, array), expected);
}

TEST(JSONLD_expand, document_context_overrides_expansion_context_base) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@base": "https://doc.example/" },
    "@id": "child",
    "http://example.com/p": "v"
  })");
  const auto context = sourcemeta::core::parse_json(
      R"({ "@base": "https://ctx.example/dir/" })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@id": "https://doc.example/child",
      "http://example.com/p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, context), expected);
}

TEST(JSONLD_expand, object_definition_is_not_a_prefix) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "https://example.com/ns/" } },
    "p:x": "v",
    "https://example.com/q": { "@id": "p:y" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "p:x": [ { "@value": "v" } ],
      "https://example.com/q": [ { "@id": "p:y" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, object_definition_is_not_a_prefix_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "https://example.com/ns/" } },
    "p:x": "v",
    "https://example.com/q": { "@id": "p:y" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "p:x": [ { "@value": "v" } ],
      "https://example.com/q": [ { "@id": "p:y" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, "", {}, sourcemeta::core::JSONLDVersion::V1_0),
            expected);
}

TEST(JSONLD_expand, simple_definition_is_a_prefix) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": "https://example.com/ns/" },
    "p:x": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "https://example.com/ns/x": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, "", {}, sourcemeta::core::JSONLDVersion::V1_0),
            expected);
}

TEST(JSONLD_expand, empty_type_array_preserved) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@type": [],
    "https://example.com/p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@type": [],
      "https://example.com/p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input,
                                            sourcemeta::core::parse_json("{}")),
            expected);
}

TEST(JSONLD_expand, empty_type_array_preserved_through_alias) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "type": "@type" },
    "type": [],
    "https://example.com/p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "@type": [],
      "https://example.com/p": [ { "@value": "v" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, remote_context_loaded_once) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView identifier)
      -> std::optional<sourcemeta::core::JSON> {
    if (identifier == "https://example.com/shared") {
      invocations += 1;
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "http://example.com/p" } })");
    }
    return std::nullopt;
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      "https://example.com/shared",
      { "@vocab": "http://example.com/vocab/" },
      "https://example.com/shared"
    ],
    "p": { "@context": "https://example.com/shared", "p": "v" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "http://example.com/p": [
        { "http://example.com/p": [ { "@value": "v" } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), expected);
  // A previously dereferenced context is never dereferenced again within one
  // expansion
  EXPECT_EQ(invocations, 1);
}

TEST(JSONLD_expand, remote_context_reuse_keeps_first_document) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    if (invocations == 1) {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "http://example.com/first" } })");
    }
    return sourcemeta::core::parse_json(
        R"({ "@context": { "p": "http://example.com/second" } })");
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      "https://example.com/stateful",
      {},
      "https://example.com/stateful"
    ],
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/first": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), expected);
  EXPECT_EQ(invocations, 1);

  // A later expansion call starts fresh and observes the new document
  const auto second_expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/second": [ { "@value": "v" } ] }
  ])");
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver),
            second_expected);
}

TEST(JSONLD_expand, index_map_none_alias_suppresses_metadata) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "none": "@none",
      "p": { "@id": "https://example.com/p", "@container": "@index" }
    },
    "p": { "none": "v", "other": "w" }
  })");

  auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        { "@value": "v" },
        { "@value": "w", "@index": "other" }
      ]
    }
  ])");

  auto result = sourcemeta::core::jsonld_expand(input);

  // Expanded values of an ordinary property are an unordered set, so the
  // comparison must not depend on emission order
  auto &result_values = result.at(0).at("https://example.com/p");
  std::sort(result_values.as_array().begin(), result_values.as_array().end());
  auto &expected_values = expected.at(0).at("https://example.com/p");
  std::sort(expected_values.as_array().begin(),
            expected_values.as_array().end());

  EXPECT_EQ(result, expected);
}

TEST(JSONLD_expand, property_valued_index_map_none_alias) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "none": "@none",
      "p": {
        "@id": "https://example.com/p",
        "@container": "@index",
        "@index": "https://example.com/prop"
      }
    },
    "p": { "none": { "@id": "https://example.com/node" } }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        { "@id": "https://example.com/node" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, type_container_defaults_to_identifier_coercion) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "https://example.com/p", "@container": "@type" }
    },
    "p": "https://example.com/node"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        { "@id": "https://example.com/node" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);

  const auto array_input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "https://example.com/p", "@container": "@type" }
    },
    "p": [ "https://example.com/a", "https://example.com/b" ]
  })");

  auto array_expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        { "@id": "https://example.com/a" },
        { "@id": "https://example.com/b" }
      ]
    }
  ])");

  auto array_result = sourcemeta::core::jsonld_expand(array_input);

  // Expanded values of an ordinary property are an unordered set, so the
  // comparison must not depend on emission order
  auto &result_values = array_result.at(0).at("https://example.com/p");
  std::sort(result_values.as_array().begin(), result_values.as_array().end());
  auto &expected_values = array_expected.at(0).at("https://example.com/p");
  std::sort(expected_values.as_array().begin(),
            expected_values.as_array().end());

  EXPECT_EQ(array_result, array_expected);
}

TEST(JSONLD_expand, type_container_explicit_vocab_mapping_is_kept) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "@vocab": "https://example.com/vocab/",
      "p": {
        "@id": "https://example.com/p",
        "@container": "@type",
        "@type": "@vocab"
      }
    },
    "p": "node"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        { "@id": "https://example.com/vocab/node" }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, protected_type_container_redefinition_with_explicit_id) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      {
        "@protected": true,
        "p": { "@id": "https://example.com/p", "@container": "@type" }
      },
      {
        "p": {
          "@id": "https://example.com/p",
          "@container": "@type",
          "@type": "@id"
        }
      }
    ],
    "p": { "https://example.com/T": "https://example.com/node" }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        {
          "@id": "https://example.com/node",
          "@type": [ "https://example.com/T" ]
        }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, protected_only_type_keyword_definition) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "@type": { "@protected": true } },
    "@type": "urn:T"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "@type": [ "urn:T" ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);

  const auto unprotected_input = sourcemeta::core::parse_json(R"({
    "@context": { "@type": { "@protected": false } },
    "@type": "urn:T"
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(unprotected_input), expected);
}

TEST(JSONLD_expand, reverse_defined_keyword_value_is_ignored) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@reverse": "@id" } },
    "http://example.com/q": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/q": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, nested_raw_array_in_explicit_list_flattens) {
  const auto input = sourcemeta::core::parse_json(R"({
    "https://example.com/p": { "@list": [ [ 1 ] ] }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        { "@list": [ { "@value": 1 } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, nested_list_object_in_explicit_list_is_preserved) {
  const auto input = sourcemeta::core::parse_json(R"({
    "https://example.com/p": { "@list": [ { "@list": [ 1 ] } ] }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        { "@list": [ { "@list": [ { "@value": 1 } ] } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, nested_array_under_list_container_is_wrapped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "https://example.com/p", "@container": "@list" }
    },
    "p": [ [ 1 ] ]
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        { "@list": [ { "@list": [ { "@value": 1 } ] } ] }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, scalar_scoped_context_overrides_protected_term) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "x": { "@id": "urn:x", "@protected": true } },
      { "p": { "@id": "urn:p", "@context": { "x": "urn:y" } } }
    ],
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);

  const auto array_input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "x": { "@id": "urn:x", "@protected": true } },
      { "p": { "@id": "urn:p", "@context": { "x": "urn:y" } } }
    ],
    "p": [ "v" ]
  })");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(array_input), expected);
}

TEST(JSONLD_expand, default_framing_keyword_type_is_dropped) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@type": "@default",
    "https://example.com/p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "https://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, default_framing_keyword_as_coerced_identifier) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": { "@id": "https://example.com/p", "@type": "@id" }
    },
    "p": "@default"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "https://example.com/p": [ { "@id": null } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, graph_container_with_single_map_kind_and_set) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "p": {
        "@id": "https://example.com/p",
        "@container": [ "@graph", "@index", "@set" ]
      }
    },
    "p": { "i": { "https://example.com/q": "v" } }
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "https://example.com/p": [
        {
          "@graph": [ { "https://example.com/q": [ { "@value": "v" } ] } ],
          "@index": "i"
        }
      ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, unprotected_context_nullification_mid_array) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [ { "a": "urn:a" }, null ],
    "urn:p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, imported_context_loaded_once) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    if (invocations == 1) {
      return sourcemeta::core::parse_json(
          R"({ "@context": { "p": "http://example.com/first" } })");
    }
    return sourcemeta::core::parse_json(
        R"({ "@context": { "p": "http://example.com/second" } })");
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "@import": "https://example.com/import" },
      { "@import": "https://example.com/import" }
    ],
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/first": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), expected);
  EXPECT_EQ(invocations, 1);
}

TEST(JSONLD_expand, direct_and_import_references_share_the_cache) {
  std::size_t invocations{0};
  const sourcemeta::core::JSONLDResolver resolver =
      [&invocations](const sourcemeta::core::JSON::StringView)
      -> std::optional<sourcemeta::core::JSON> {
    invocations += 1;
    return sourcemeta::core::parse_json(
        R"({ "@context": { "p": "http://example.com/p" } })");
  };

  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      "https://example.com/shared",
      { "@import": "https://example.com/shared" }
    ],
    "p": "v"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "http://example.com/p": [ { "@value": "v" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input, "", resolver), expected);
  EXPECT_EQ(invocations, 1);
}

TEST(JSONLD_expand, type_scoped_precedence_follows_input_keys) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "A": { "@id": "urn:A", "@context": { "p": "urn:fromA" } },
      "Z": { "@id": "urn:Z", "@context": { "p": "urn:fromZ" } },
      "t": "@type"
    },
    "@type": "Z",
    "t": "A",
    "p": "x"
  })");

  auto expected = sourcemeta::core::parse_json(R"([
    {
      "@type": [ "urn:Z", "urn:A" ],
      "urn:fromA": [ { "@value": "x" } ]
    }
  ])");

  auto result = sourcemeta::core::jsonld_expand(input);

  // The @type values form an unordered set, so the comparison must not depend
  // on emission order
  std::sort(result.at(0).at("@type").as_array().begin(),
            result.at(0).at("@type").as_array().end());
  std::sort(expected.at(0).at("@type").as_array().begin(),
            expected.at(0).at("@type").as_array().end());

  EXPECT_EQ(result, expected);
}

TEST(JSONLD_expand, type_scoped_precedence_sorts_values_within_entry) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": {
      "A": { "@id": "urn:A", "@context": { "p": "urn:fromA" } },
      "Z": { "@id": "urn:Z", "@context": { "p": "urn:fromZ" } }
    },
    "@type": [ "Z", "A" ],
    "p": "x"
  })");

  auto expected = sourcemeta::core::parse_json(R"([
    {
      "@type": [ "urn:Z", "urn:A" ],
      "urn:fromZ": [ { "@value": "x" } ]
    }
  ])");

  auto result = sourcemeta::core::jsonld_expand(input);

  // The @type values form an unordered set, so the comparison must not depend
  // on emission order
  std::sort(result.at(0).at("@type").as_array().begin(),
            result.at(0).at("@type").as_array().end());
  std::sort(expected.at(0).at("@type").as_array().begin(),
            expected.at(0).at("@type").as_array().end());

  EXPECT_EQ(result, expected);
}

TEST(JSONLD_expand, term_direction_accepted_in_1_0) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": { "p": { "@id": "urn:p", "@direction": "rtl" } },
    "p": "x"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:p": [ { "@value": "x", "@direction": "rtl" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(
                input, "", {}, sourcemeta::core::JSONLDVersion::V1_0),
            expected);
}

TEST(JSONLD_expand, explicit_unprotected_type_definition_is_redefinable) {
  const auto input = sourcemeta::core::parse_json(R"({
    "@context": [
      { "@protected": true, "@type": { "@protected": false } },
      { "@type": { "@container": "@set" } }
    ],
    "@type": "urn:T"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "@type": [ "urn:T" ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, null_set_drops_property) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": { "@set": null },
    "urn:q": "x"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    { "urn:q": [ { "@value": "x" } ] }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, empty_set_keeps_empty_property) {
  const auto input = sourcemeta::core::parse_json(R"({
    "urn:p": { "@set": [] },
    "urn:q": "x"
  })");

  const auto expected = sourcemeta::core::parse_json(R"([
    {
      "urn:p": [],
      "urn:q": [ { "@value": "x" } ]
    }
  ])");

  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand, error_message_survives_buffer_destruction_and_copy) {
  std::optional<sourcemeta::core::JSONLDError> copy;

  {
    std::string code{
        "A custom error code longer than small string optimization"};
    const sourcemeta::core::JSONLDError error{
        code.c_str(), sourcemeta::core::Pointer{"where"}};
    copy.emplace(error);
  }

  EXPECT_STREQ(copy->what(),
               "A custom error code longer than small string optimization");
  EXPECT_EQ(sourcemeta::core::to_string(copy->pointer()), "/where");
}
