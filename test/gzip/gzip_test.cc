#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

#include <sstream>

TEST(GZIP, compress_string_1) {
  const auto result{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 31);
}

TEST(GZIP, decompress_stream_1) {
  const auto input{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "Hello World");
}

TEST(GZIP, decompress_stream_empty) {
  const auto input{sourcemeta::core::gzip("")};
  EXPECT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(GZIP, decompress_stream_long_string) {
  const std::string long_text(10000, 'A');
  const auto input{sourcemeta::core::gzip(long_text)};
  EXPECT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), long_text);
}

TEST(GZIP, decompress_stream_invalid_data) {
  std::istringstream stream{"This is not gzipped data"};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}
