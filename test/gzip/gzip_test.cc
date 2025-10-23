#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

#include <sstream> // std::istringstream

TEST(GZIP, compress_string_1) {
  const auto result{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 31);
}

TEST(GZIP, decompress_stream_1) {
  const auto input{sourcemeta::core::gzip("Hello World")};
  ASSERT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "Hello World");
}

TEST(GZIP, decompress_stream_empty) {
  const auto input{sourcemeta::core::gzip("")};
  ASSERT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(GZIP, decompress_stream_long_text) {
  const std::string long_text(10000, 'A');
  const auto input{sourcemeta::core::gzip(long_text)};
  ASSERT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), long_text);
}

TEST(GZIP, decompress_stream_invalid_data) {
  std::istringstream stream{"not a valid gzip stream"};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}

TEST(GZIP, compress_and_decompress_roundtrip) {
  const std::string original{"The quick brown fox jumps over the lazy dog"};
  const auto compressed{sourcemeta::core::gzip(original)};
  ASSERT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto decompressed{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(decompressed.has_value());
  EXPECT_EQ(decompressed.value(), original);
}
