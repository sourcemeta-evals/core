#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>

TEST(JSONLDTest, expand_wraps_document) {
  auto input = sourcemeta::core::parse_json("{\"foo\": \"bar\"}");
  auto result = sourcemeta::core::jsonld_expand(input);
  EXPECT_TRUE(result.is_array());
  EXPECT_EQ(result.size(), 1);
}
