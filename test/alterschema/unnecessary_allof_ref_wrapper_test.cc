#include "alterschema_test_utils.h"
#include <gtest/gtest.h>
#include <sourcemeta/core/alterschema.h>

using namespace sourcemeta::core;

void assert_lint(const std::string &input, const std::string &expected_output,
                 sourcemeta::core::AlterSchemaMode mode) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json(input);
  sourcemeta::core::SchemaTransformer bundle;
  sourcemeta::core::add(bundle, mode);
  bundle.apply(document, sourcemeta::core::schema_official_walker,
               sourcemeta::core::schema_official_resolver,
               [](const auto &, const auto &, const auto &, const auto &) {});
  const sourcemeta::core::JSON expected =
      sourcemeta::core::parse_json(expected_output);
  EXPECT_EQ(document, expected);
}

TEST(AlterSchemaLint, UnnecessaryAllOfRefWrapper) {
  // Test case 1: Basic extraction
  assert_lint(
      R"({
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "allOf": [
          { "$ref": "https://example.com" }
        ]
      })",
      R"({
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$ref": "https://example.com"
      })",
      AlterSchemaMode::Readability);

  // Test case 2: Extraction with other branches
  assert_lint(
      R"({
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "allOf": [
          { "$ref": "https://example.com" },
          { "type": "string" }
        ]
      })",
      R"({
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$ref": "https://example.com",
        "allOf": [
          { "type": "string" }
        ]
      })",
      AlterSchemaMode::Readability);

  // Test case 3: No extraction if parent has $ref
  assert_lint(
      R"({
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$ref": "https://other.com",
        "allOf": [
          { "$ref": "https://example.com" }
        ]
      })",
      R"({
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$ref": "https://other.com",
        "allOf": [
          { "$ref": "https://example.com" }
        ]
      })",
      AlterSchemaMode::Readability);

  // Test case 4: No extraction if multiple branches have $ref
  assert_lint(
      R"({
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "allOf": [
          { "$ref": "https://example.com" },
          { "$ref": "https://example2.com" }
        ]
      })",
      R"({
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "allOf": [
          { "$ref": "https://example.com" },
          { "$ref": "https://example2.com" }
        ]
      })",
      AlterSchemaMode::Readability);

  // Test case 5: Extraction with extra keywords in branch
  assert_lint(
      R"({
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "allOf": [
          { "$ref": "https://example.com", "description": "foo" }
        ]
      })",
      R"({
        "$schema": "https://json-schema.org/draft/2020-12/schema",
        "$ref": "https://example.com",
        "allOf": [
          { "description": "foo" }
        ]
      })",
      AlterSchemaMode::Readability);

  // Test case 6: Draft 2019-09 support
  assert_lint(
      R"({
        "$schema": "https://json-schema.org/draft/2019-09/schema",
        "allOf": [
          { "$ref": "https://example.com" }
        ]
      })",
      R"({
        "$schema": "https://json-schema.org/draft/2019-09/schema",
        "$ref": "https://example.com"
      })",
      AlterSchemaMode::Readability);

  // Test case 7: Draft 7 ignored (legacy behavior)
  assert_lint(
      R"({
        "$schema": "http://json-schema.org/draft-07/schema#",
        "allOf": [
          { "$ref": "https://example.com" }
        ]
      })",
      R"({
        "$schema": "http://json-schema.org/draft-07/schema#",
        "allOf": [
          { "$ref": "https://example.com" }
        ]
      })",
      AlterSchemaMode::Readability);
}
