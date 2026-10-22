#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include "alterschema_test_utils.h"

namespace {

using Traces = std::vector<std::tuple<sourcemeta::core::Pointer, std::string,
                                      std::string, std::string>>;

auto check_for_readability(
    const sourcemeta::core::JSON &document, Traces &traces,
    const std::optional<std::string> &default_dialect = std::nullopt) -> bool {
  sourcemeta::core::SchemaTransformer bundle;
  sourcemeta::core::add(bundle, sourcemeta::core::AlterSchemaMode::Readability);
  return bundle.check(
      document, sourcemeta::core::schema_official_walker,
      sourcemeta::core::schema_official_resolver,
      [&traces](const auto &pointer, const auto &name, const auto &message,
                const auto &description) {
        traces.emplace_back(pointer, name, message, description);
      },
      default_dialect);
}

auto check_for_static_analysis(const sourcemeta::core::JSON &document,
                               Traces &traces) -> bool {
  sourcemeta::core::SchemaTransformer bundle;
  sourcemeta::core::add(bundle,
                        sourcemeta::core::AlterSchemaMode::StaticAnalysis);
  return bundle.check(document, sourcemeta::core::schema_official_walker,
                      sourcemeta::core::schema_official_resolver,
                      [&traces](const auto &pointer, const auto &name,
                                const auto &message, const auto &description) {
                        traces.emplace_back(pointer, name, message,
                                            description);
                      });
}

auto fix_for_readability(
    sourcemeta::core::JSON &document,
    const std::optional<std::string> &default_dialect = std::nullopt) -> bool {
  sourcemeta::core::SchemaTransformer bundle;
  sourcemeta::core::add(bundle, sourcemeta::core::AlterSchemaMode::Readability);
  return bundle.apply(
      document, sourcemeta::core::schema_official_walker,
      sourcemeta::core::schema_official_resolver,
      [](const auto &, const auto &name, const auto &, const auto &) {
        ADD_FAILURE() << name;
      },
      default_dialect);
}

} // namespace

TEST(AlterSchema_lint_2019_09, enum_to_const_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "enum": [ 1 ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "const": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, const_with_type_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "string",
    "const": "foo"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "const": "foo"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, enum_with_type_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "string",
    "enum": [ "foo", "bar" ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "enum": [ "foo", "bar" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, single_type_array_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": [ "string" ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "string"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, content_media_type_without_encoding_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "contentMediaType": "application/json"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, content_schema_without_media_type_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "contentEncoding": "base64",
    "contentSchema": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "contentEncoding": "base64"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, max_contains_without_contains_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "maxContains": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, max_contains_without_contains_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "contains": true,
    "maxContains": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "contains": true,
    "maxContains": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, min_contains_without_contains_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "minContains": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, min_contains_without_contains_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "contains": true,
    "minContains": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "contains": true,
    "minContains": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, then_without_if_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "then": true
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, then_without_if_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "if": true,
    "then": true
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "if": true,
    "then": true
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, else_without_if_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "else": true
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, else_without_if_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "if": true,
    "else": true
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "if": true,
    "else": true
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, if_without_then_else_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "if": true
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unevaluated_properties_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "unevaluatedProperties": true
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unevaluated_properties_default_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "unevaluatedProperties": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unevaluated_properties_default_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "unevaluatedProperties": { "type": "string" }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "unevaluatedProperties": { "type": "string" }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unevaluated_items_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "unevaluatedItems": true
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unevaluated_items_default_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "unevaluatedItems": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unevaluated_items_default_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "unevaluatedItems": { "type": "string" }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "unevaluatedItems": { "type": "string" }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, additional_properties_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "additionalProperties": true
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, additional_properties_default_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "additionalProperties": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, additional_properties_default_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "additionalProperties": false
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "additionalProperties": false
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, content_schema_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "contentEncoding": "base64",
    "contentMediaType": "application/json",
    "contentSchema": true
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "contentEncoding": "base64",
    "contentMediaType": "application/json"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, content_schema_default_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "contentEncoding": "base64",
    "contentMediaType": "application/json",
    "contentSchema": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "contentEncoding": "base64",
    "contentMediaType": "application/json"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, content_schema_default_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "contentEncoding": "base64",
    "contentMediaType": "application/json",
    "contentSchema": { "type": "string" }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "contentEncoding": "base64",
    "contentMediaType": "application/json",
    "contentSchema": { "type": "string" }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, items_schema_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "items": true
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, items_schema_default_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "items": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, items_schema_default_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "items": false
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "items": false
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, items_array_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "items": []
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, duplicate_enum_values_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "enum": [ 1, {}, 2, 1, 1, 3, {} ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "enum": [ 1, 2, 3, {} ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, duplicate_required_values_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "required": [ "foo", "bar", "baz", "foo" ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "required": [ "bar", "baz", "foo" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, exclusive_maximum_number_and_maximum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "exclusiveMaximum": 3,
    "maximum": 3
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "exclusiveMaximum": 3
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, exclusive_maximum_number_and_maximum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "exclusiveMaximum": 3,
    "maximum": 2
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "maximum": 2
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, exclusive_maximum_number_and_maximum_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "exclusiveMaximum": 3,
    "maximum": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "maximum": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, exclusive_minimum_number_and_minimum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "exclusiveMinimum": 3,
    "minimum": 3
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "exclusiveMinimum": 3
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, exclusive_minimum_number_and_minimum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "exclusiveMinimum": 3,
    "minimum": 2
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "exclusiveMinimum": 3
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, exclusive_minimum_number_and_minimum_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "exclusiveMinimum": 3,
    "minimum": 4
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "minimum": 4
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, duplicate_allof_branches_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [ { "type": "string" }, { "type": "integer" }, { "type": "string" } ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [ { "type": "integer" }, { "type": "string" } ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, duplicate_anyof_branches_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "anyOf": [ { "type": "string" }, { "type": "integer" }, { "type": "string" } ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "anyOf": [ { "type": "integer" }, { "type": "string" } ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, maximum_real_for_integer_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "maximum": 3.2
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "maximum": 3
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, minimum_real_for_integer_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "minimum": 3.2
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "minimum": 4
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, dependent_required_tautology_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "required": [ "foo" ],
    "dependentRequired": {
      "foo": [ "bar" ],
      "xxx": [ "yyy" ]
    }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "required": [ "foo", "bar" ],
    "dependentRequired": {
      "xxx": [ "yyy" ]
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, dependent_required_tautology_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "required": [ "foo" ],
    "dependentRequired": {
      "foo": [ "bar" ],
      "bar": [ "baz" ]
    }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "required": [ "foo", "bar", "baz" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, properties_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "properties": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, pattern_properties_default_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "patternProperties": {}
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unsatisfiable_min_properties_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "required": [ "foo", "bar", "bar", "baz" ],
    "minProperties": 3
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "required": [ "bar", "baz", "foo" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unsatisfiable_min_properties_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "required": [ "foo", "bar", "bar", "baz" ],
    "minProperties": 4
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "required": [ "bar", "baz", "foo" ],
    "minProperties": 4
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unsatisfiable_min_properties_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "required": [ "foo", "bar", "bar", "baz" ],
    "minProperties": 2
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "required": [ "bar", "baz", "foo" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, drop_non_array_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "title": "Test",
    "type": "array",
    "unevaluatedProperties": false,
    "contentEncoding": "base64",
    "maxProperties": 3,
    "format": "uri",
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "title": "Test",
    "type": "array",
    "minItems": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, drop_non_boolean_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "title": "Test",
    "type": "boolean",
    "unevaluatedProperties": false,
    "contentEncoding": "base64",
    "maxProperties": 3,
    "format": "uri",
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "title": "Test",
    "type": "boolean"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, drop_non_null_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "title": "Test",
    "type": "null",
    "unevaluatedProperties": false,
    "contentEncoding": "base64",
    "maxProperties": 3,
    "format": "uri",
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "title": "Test",
    "type": "null"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, drop_non_numeric_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "title": "Test",
    "type": "number",
    "multipleOf": 2,
    "unevaluatedProperties": false,
    "contentEncoding": "base64",
    "maxProperties": 3,
    "format": "uri",
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "title": "Test",
    "type": "number",
    "multipleOf": 2
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, drop_non_numeric_keywords_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "title": "Test",
    "type": "integer",
    "multipleOf": 2,
    "unevaluatedProperties": false,
    "contentEncoding": "base64",
    "maxProperties": 3,
    "format": "uri",
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "title": "Test",
    "type": "integer",
    "multipleOf": 2
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, drop_non_object_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "title": "Test",
    "type": "object",
    "multipleOf": 2,
    "unevaluatedProperties": false,
    "contentEncoding": "base64",
    "maxProperties": 3,
    "format": "uri",
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "title": "Test",
    "type": "object",
    "unevaluatedProperties": false,
    "maxProperties": 3
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, drop_non_string_keywords_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "title": "Test",
    "type": "string",
    "multipleOf": 2,
    "unevaluatedProperties": false,
    "contentEncoding": "base64",
    "maxProperties": 3,
    "format": "uri",
    "minItems": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "title": "Test",
    "type": "string",
    "contentEncoding": "base64",
    "format": "uri"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, type_boolean_as_enum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "boolean"
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "enum": [ false, true ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, type_boolean_as_enum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "boolean",
    "enum": [ 1, 2, 3 ]
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "boolean",
    "enum": [ 1, 2, 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, type_boolean_as_enum_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "boolean"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "boolean"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, type_null_as_enum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "null"
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "enum": [ null ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, type_null_as_enum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "null",
    "enum": [ 1, 2, 3 ]
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "null",
    "enum": [ 1, 2, 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, type_null_as_enum_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "null"
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "null"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, const_as_enum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "const": 1
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "enum": [ 1 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, const_as_enum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "const": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "const": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, exclusive_maximum_integer_to_maximum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "exclusiveMaximum": 1
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "multipleOf": 1,
    "maximum": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, exclusive_maximum_integer_to_maximum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "exclusiveMaximum": 1.2
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "multipleOf": 1,
    "maximum": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, exclusive_maximum_integer_to_maximum_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "number",
    "exclusiveMaximum": 1
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "number",
    "multipleOf": 1,
    "exclusiveMaximum": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, exclusive_maximum_integer_to_maximum_4) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "exclusiveMaximum": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "exclusiveMaximum": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, exclusive_maximum_integer_to_maximum_5) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "exclusiveMaximum": 1
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": [ "null", "boolean", "object", "array", "string", "number", "integer" ],
    "exclusiveMaximum": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, exclusive_minimum_integer_to_minimum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "exclusiveMinimum": 1
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "multipleOf": 1,
    "minimum": 2
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, exclusive_minimum_integer_to_minimum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "exclusiveMinimum": 1.2
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "multipleOf": 1,
    "minimum": 2
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, exclusive_minimum_integer_to_minimum_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "number",
    "exclusiveMinimum": 1
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "number",
    "multipleOf": 1,
    "exclusiveMinimum": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, exclusive_minimum_integer_to_minimum_4) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "exclusiveMinimum": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "exclusiveMinimum": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, exclusive_minimum_integer_to_minimum_5) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "exclusiveMinimum": 1
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": [ "null", "boolean", "object", "array", "string", "number", "integer" ],
    "exclusiveMinimum": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unsatisfiable_max_contains_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "array",
    "contains": { "type": "string" },
    "maxItems": 3,
    "maxContains": 3
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "array",
    "contains": { "type": "string" },
    "maxItems": 3
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unsatisfiable_max_contains_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "array",
    "contains": { "type": "string" },
    "maxItems": 3,
    "maxContains": 1
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "array",
    "contains": { "type": "string" },
    "maxItems": 3,
    "maxContains": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unsatisfiable_max_contains_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "array",
    "contains": { "type": "string" },
    "maxItems": 3,
    "maxContains": 4
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "array",
    "contains": { "type": "string" },
    "maxItems": 3
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, boolean_true_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "properties": {
      "foo": true
    }
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": [ "null", "boolean", "object", "array", "string", "number", "integer" ],
    "properties": {
      "foo": {
        "type": [ "null", "boolean", "object", "array", "string", "number", "integer" ]
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, max_contains_covered_by_max_items_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "array",
    "contains": { "type": "string" },
    "maxContains": 2,
    "maxItems": 1
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "array",
    "contains": {
      "type": "string",
      "minLength": 0
    },
    "maxContains": 1,
    "minItems": 0,
    "maxItems": 1
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, min_properties_covered_by_required_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "object",
    "minProperties": 1,
    "required": [ "foo", "bar" ]
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "object",
    "minProperties": 2,
    "properties": {},
    "required": [ "foo", "bar" ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, min_properties_implicit_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "required": [ "foo", "bar" ]
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object",
    "required": [ "foo", "bar" ],
    "properties": {},
    "minProperties": 2
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, min_properties_implicit_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "object",
    "minProperties": 2,
    "required": [ "foo", "bar" ]
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "object",
    "properties": {},
    "required": [ "foo", "bar" ],
    "minProperties": 2
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, min_items_given_min_contains_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "array",
    "contains": { "type": "boolean" },
    "minContains": 3
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "array",
    "contains": { "enum": [ false, true ] },
    "minContains": 3,
    "minItems": 3
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, min_items_given_min_contains_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "array",
    "minContains": 3
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "array",
    "minItems": 0
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, equal_numeric_bounds_to_enum_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "minimum": 3,
    "maximum": 3
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "const": 3
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, equal_numeric_bounds_to_enum_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "type": "integer",
    "minimum": 3,
    "maximum": 3
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "enum": [ 3 ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_1) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      { "$ref": "https://example.com" }
    ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$ref": "https://example.com"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_2) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      { "$ref": "https://example.com/foo" },
      { "$ref": "https://example.com/bar" }
    ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      { "$ref": "https://example.com/foo" },
      { "$ref": "https://example.com/bar" }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_3) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$ref": "https://example.com/foo",
    "allOf": [
      { "$ref": "https://example.com/bar" }
    ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$ref": "https://example.com/foo",
    "allOf": [
      { "$ref": "https://example.com/bar" }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_4) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      { "type": "number" },
      { "$ref": "https://example.com" },
      { "type": "integer" }
    ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$ref": "https://example.com",
    "allOf": [
      { "type": "number" },
      { "type": "integer" }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_5) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      { 
        "type": "integer",
        "$ref": "https://example.com"
      }
    ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$ref": "https://example.com",
    "allOf": [
      { "type": "integer" }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_6) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      {
        "$id": "https://example.com/nested",
        "$ref": "https://example.com"
      }
    ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      {
        "$id": "https://example.com/nested",
        "$ref": "https://example.com"
      }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_7) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      {
        "unevaluatedProperties": false,
        "$ref": "https://example.com"
      }
    ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      {
        "unevaluatedProperties": false,
        "$ref": "https://example.com"
      }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_8) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$defs": {
      "indirect": { "$ref": "#/allOf/0" }
    },
    "allOf": [
      { "$ref": "https://example.com" }
    ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$defs": {
      "indirect": { "$ref": "#/allOf/0" }
    },
    "allOf": [
      { "$ref": "https://example.com" }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_9) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$defs": {
      "indirect": { "$ref": "#/allOf/1" }
    },
    "allOf": [
      { "$ref": "https://example.com" },
      { "type": "string" },
      { "minLength": 1 }
    ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$defs": {
      "indirect": { "$ref": "#/allOf/1" }
    },
    "allOf": [
      { "$ref": "https://example.com" },
      { "type": "string" },
      { "minLength": 1 }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_check_1) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$defs": {
      "indirect": { "$ref": "#/allOf/0" }
    },
    "allOf": [
      { "$ref": "https://example.com" }
    ]
  })JSON");

  Traces entries;
  const auto result{check_for_readability(document, entries)};

  EXPECT_TRUE(result);
  EXPECT_TRUE(entries.empty());
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_check_2) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      { "$ref": "https://example.com" }
    ]
  })JSON");

  Traces entries;
  const auto result{check_for_readability(document, entries)};

  EXPECT_FALSE(result);
  EXPECT_EQ(entries.size(), 1);

  EXPECT_EQ(std::get<0>(entries.at(0)), sourcemeta::core::Pointer{});
  EXPECT_EQ(std::get<1>(entries.at(0)), "unnecessary_allof_ref_wrapper");
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_10) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      { "$ref": "https://example.com/foo" },
      { "type": "string" },
      { "$ref": "https://example.com/bar" }
    ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      { "$ref": "https://example.com/foo" },
      { "type": "string" },
      { "$ref": "https://example.com/bar" }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_11) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      {
        "$recursiveAnchor": true,
        "$ref": "https://example.com"
      }
    ]
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      {
        "$recursiveAnchor": true,
        "$ref": "https://example.com"
      }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_12) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      { "$ref": "https://example.com" }
    ]
  })JSON");

  LINT_AND_FIX_FOR_STATIC_ANALYSIS(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$ref": "https://example.com"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_13) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$defs": {
      "indirect": { "$ref": "#/properties/foo/allOf/0" }
    },
    "properties": {
      "foo": {
        "allOf": [
          { "$ref": "https://example.com" }
        ]
      }
    }
  })JSON");

  LINT_AND_FIX_FOR_READABILITY(document);

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$defs": {
      "indirect": { "$ref": "#/properties/foo/allOf/0" }
    },
    "properties": {
      "foo": {
        "allOf": [
          { "$ref": "https://example.com" }
        ]
      }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_14) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$id": "https://example.com/schema",
    "$defs": {
      "indirect": { "$ref": "#keep" }
    },
    "allOf": [
      { "$ref": "https://example.com/target" },
      { "$anchor": "keep", "type": "string" }
    ]
  })JSON");

  EXPECT_TRUE(fix_for_readability(document));

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$id": "https://example.com/schema",
    "$defs": {
      "indirect": { "$ref": "#keep" }
    },
    "allOf": [
      { "$ref": "https://example.com/target" },
      { "$anchor": "keep", "type": "string" }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_15) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$defs": {
      "value": { "type": "string" }
    },
    "allOf": [
      { "$ref": "#/$defs/value" }
    ]
  })JSON");

  EXPECT_TRUE(fix_for_readability(document));

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$ref": "#/$defs/value",
    "$defs": {
      "value": { "type": "string" }
    }
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_16) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "examples": [ { "$ref": "#/allOf/0" } ],
    "allOf": [
      { "$ref": "https://example.com/target" }
    ]
  })JSON");

  EXPECT_TRUE(fix_for_readability(document));

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$ref": "https://example.com/target",
    "examples": [ { "$ref": "#/allOf/0" } ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_17) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      { "$ref": "https://example.com/remote#/$defs/184467440737095516160" }
    ]
  })JSON");

  EXPECT_TRUE(fix_for_readability(document));

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$ref": "https://example.com/remote#/$defs/184467440737095516160"
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_18) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$defs": {
      "value": { "type": "string" },
      "indirect": { "$ref": "#deep" }
    },
    "allOf": [
      { "$ref": "#/$defs/value" },
      {
        "properties": {
          "foo": { "$anchor": "deep", "type": "integer" }
        }
      }
    ]
  })JSON");

  EXPECT_TRUE(fix_for_readability(document));

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "$defs": {
      "value": { "type": "string" },
      "indirect": { "$ref": "#deep" }
    },
    "allOf": [
      { "$ref": "#/$defs/value" },
      {
        "properties": {
          "foo": { "$anchor": "deep", "type": "integer" }
        }
      }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_19) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$defs": {
      "value": { "type": "string" }
    },
    "allOf": [
      { "$ref": "#/$defs/value", "title": "Sibling" }
    ]
  })JSON");

  EXPECT_TRUE(fix_for_readability(
      document, "https://json-schema.org/draft/2019-09/schema"));

  const sourcemeta::core::JSON expected = sourcemeta::core::parse_json(R"JSON({
    "$ref": "#/$defs/value",
    "$defs": {
      "value": { "type": "string" }
    },
    "allOf": [
      { "title": "Sibling" }
    ]
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_check_3) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      { "$ref": "https://example.com", "title": "Sibling" }
    ]
  })JSON");

  Traces entries;
  const auto result{check_for_static_analysis(document, entries)};

  EXPECT_FALSE(result);
  EXPECT_EQ(entries.size(), 1);

  EXPECT_EQ(std::get<0>(entries.at(0)), sourcemeta::core::Pointer{});
  EXPECT_EQ(std::get<1>(entries.at(0)), "unnecessary_allof_ref_wrapper");
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_check_4) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$defs": {
      "value": { "type": "string" }
    },
    "allOf": [
      { "$ref": "#/$defs/value", "title": "Sibling" }
    ]
  })JSON");

  Traces entries;
  const auto result{check_for_readability(
      document, entries, "https://json-schema.org/draft/2019-09/schema")};

  EXPECT_FALSE(result);
  EXPECT_EQ(entries.size(), 1);

  EXPECT_EQ(std::get<0>(entries.at(0)), sourcemeta::core::Pointer{});
  EXPECT_EQ(std::get<1>(entries.at(0)), "unnecessary_allof_ref_wrapper");
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_check_5) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      {
        "$id": "https://example.com/nested",
        "$ref": "https://example.com"
      }
    ]
  })JSON");

  Traces entries;
  const auto result{check_for_readability(document, entries)};

  EXPECT_TRUE(result);
  EXPECT_TRUE(entries.empty());
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_check_6) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      {
        "unevaluatedProperties": false,
        "$ref": "https://example.com"
      }
    ]
  })JSON");

  Traces entries;
  const auto result{check_for_readability(document, entries)};

  EXPECT_TRUE(result);
  EXPECT_TRUE(entries.empty());
}

TEST(AlterSchema_lint_2019_09, unnecessary_allof_ref_wrapper_check_7) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2019-09/schema",
    "allOf": [
      {
        "$recursiveAnchor": true,
        "$ref": "https://example.com"
      }
    ]
  })JSON");

  Traces entries;
  const auto result{check_for_readability(document, entries)};

  EXPECT_TRUE(result);
  EXPECT_TRUE(entries.empty());
}
