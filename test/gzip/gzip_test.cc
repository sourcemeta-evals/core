#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

#include <sstream>

TEST(GZIP, compress_string_1) {
  const auto result{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 31);
}

TEST(GZIP, decompress_string_1) {
  const auto input{sourcemeta::core::gzip("Hello World")};
  ASSERT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "Hello World");
}

TEST(GZIP, compress_decompress_empty) {
  const auto compressed{sourcemeta::core::gzip("")};
  ASSERT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(GZIP, compress_decompress_long_string) {
  const std::string long_text(10000, 'A');
  const auto compressed{sourcemeta::core::gzip(long_text)};
  ASSERT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), long_text);
}

TEST(GZIP, decompress_invalid_data) {
  std::istringstream stream{"not gzip data"};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}
