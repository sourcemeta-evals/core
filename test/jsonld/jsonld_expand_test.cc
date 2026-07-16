#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>

TEST(JSONLD_expand, empty_object) {
  const auto input{sourcemeta::core::parse_json("{}")};
  const auto expected{sourcemeta::core::parse_json("[]")};
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}
