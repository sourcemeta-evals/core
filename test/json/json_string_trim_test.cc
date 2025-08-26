#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

TEST(JSON_string_trim, const_return_basic) {
  const sourcemeta::core::JSON doc{"  foo bar  "};
  const auto trimmed = doc.trim();
  EXPECT_TRUE(trimmed.is_string());
  EXPECT_EQ(trimmed.to_string(), "foo bar");
  EXPECT_EQ(doc.to_string(), "  foo bar  ");
}

TEST(JSON_string_trim, inplace_basic) {
  sourcemeta::core::JSON doc{"  foo bar  "};
  doc.trim();
  EXPECT_TRUE(doc.is_string());
  EXPECT_EQ(doc.to_string(), "foo bar");
}

TEST(JSON_string_trim, mixed_whitespace) {
  const sourcemeta::core::JSON doc{"\t\n\r\v\ffoo\t bar\r\n"};
  const auto trimmed = doc.trim();
  EXPECT_EQ(trimmed.to_string(), "foo\t bar");
}

TEST(JSON_string_trim, only_whitespace_to_empty) {
  const sourcemeta::core::JSON doc{" \t\r\n\v\f "};
  const auto trimmed = doc.trim();
  EXPECT_TRUE(trimmed.is_string());
  EXPECT_TRUE(trimmed.to_string().empty());
}

TEST(JSON_string_trim, empty_string_remains_empty) {
  const sourcemeta::core::JSON doc{""};
  const auto trimmed = doc.trim();
  EXPECT_TRUE(trimmed.is_string());
  EXPECT_TRUE(trimmed.to_string().empty());
}

TEST(JSON_string_trim, already_trimmed_unchanged) {
  sourcemeta::core::JSON doc{"foo"};
  auto copy = doc.trim();
  EXPECT_EQ(copy.to_string(), "foo");
  doc.trim();
  EXPECT_EQ(doc.to_string(), "foo");
}
