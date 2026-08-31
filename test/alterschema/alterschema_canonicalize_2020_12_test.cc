#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include "alterschema_test_utils.h"

TEST(AlterSchema_canonicalize_2020_12, duplicate_allof_branches_2) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "allOf": [
      { "type": "number" },
      { "type": "string" },
      { "type": "number" }
    ]
  })JSON");

  CANONICALIZE(document, result, traces);

  EXPECT_TRUE(result.first);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "not": true
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, duplicate_allof_branches_3) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "allOf": [
      { "type": "number" },
      { "type": "number" },
      { "type": "string" }
    ]
  })JSON");

  CANONICALIZE(document, result, traces);

  EXPECT_TRUE(result.first);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "not": true
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, duplicate_allof_branches_4) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "allOf": [
      { "type": "number" },
      { "type": "string" },
      { "type": "number" },
      { "type": "number" },
      { "type": "number" },
      { "type": "string" },
      { "type": "string" },
      { "type": "string" },
      { "type": "number" },
      { "type": "number" }
    ]
  })JSON");

  CANONICALIZE(document, result, traces);

  EXPECT_TRUE(result.first);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "not": true
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, allof_false_simplify_preserves_id) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com/schema",
    "type": "string",
    "allOf": [ false ]
  })JSON");

  CANONICALIZE(document, result, traces);

  EXPECT_TRUE(result.first);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com/schema",
    "not": true
  })JSON");

  EXPECT_EQ(document, expected);
}

TEST(AlterSchema_canonicalize_2020_12, allof_false_simplify_preserves_anchor) {
  auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$anchor": "myanchor",
    "type": "string",
    "allOf": [ false ]
  })JSON");

  CANONICALIZE(document, result, traces);

  EXPECT_TRUE(result.first);

  const auto expected = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$anchor": "myanchor",
    "not": true
  })JSON");

  EXPECT_EQ(document, expected);
}
