#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

#include <sstream> // std::istringstream

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
  const auto compressed{sourcemeta::core::gzip("")};
  EXPECT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(GZIP, decompress_stream_long_text) {
  const std::string long_text(10000, 'A');
  const auto compressed{sourcemeta::core::gzip(long_text)};
  EXPECT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), long_text);
}

TEST(GZIP, decompress_stream_invalid) {
  std::istringstream stream{"invalid gzip data"};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}
