#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

#include <sstream>

TEST(GZIP, compress_string_1) {
  const auto result{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 31);
}

TEST(GZIP, decompress_stream_1) {
  const auto compressed{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "Hello World");
}

TEST(GZIP, decompress_stream_empty) {
  std::istringstream stream{""};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}

TEST(GZIP, decompress_stream_long_string) {
  std::string long_string;
  for (int i = 0; i < 1000; ++i) {
    long_string += "This is a test string that will be repeated many times. ";
  }
  const auto compressed{sourcemeta::core::gzip(long_string)};
  EXPECT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), long_string);
}
