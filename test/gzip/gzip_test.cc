#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

#include <sstream>

TEST(GZIP, compress_string_to_string_1) {
  const auto result{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 31);
}

TEST(GZIP, decompress_stream_to_string_1) {
  const auto value{"Hello World"};
  const auto compressed{sourcemeta::core::gzip(value)};
  EXPECT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), value);
}

TEST(GZIP, decompress_stream_error_1) {
  std::istringstream input{"not-zlib-content"};
  const auto result{sourcemeta::core::gunzip(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(GZIP, roundtrip_empty_string) {
  const auto value{""};
  const auto compressed{sourcemeta::core::gzip(value)};
  EXPECT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), value);
}

TEST(GZIP, roundtrip_large_string) {
  std::string value;
  for (int i = 0; i < 10000; i++) {
    value += "Hello World ";
  }
  const auto compressed{sourcemeta::core::gzip(value)};
  EXPECT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), value);
}
