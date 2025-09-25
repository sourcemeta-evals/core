#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>
#include <sstream>

TEST(GZIP, compress_string_1) {
  const auto result{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 31);
}

TEST(GZIP, decompress_string_1) {
  const auto compressed{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(compressed.has_value());

  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "Hello World");
}

TEST(GZIP, decompress_empty_stream) {
  std::istringstream empty_stream{""};
  const auto result{sourcemeta::core::gunzip(empty_stream)};
  EXPECT_FALSE(result.has_value());
}

TEST(GZIP, decompress_invalid_data) {
  std::istringstream invalid_stream{"invalid gzip data"};
  const auto result{sourcemeta::core::gunzip(invalid_stream)};
  EXPECT_FALSE(result.has_value());
}

TEST(GZIP, round_trip_compression) {
  const std::string original{
      "This is a longer test string to verify round-trip compression and "
      "decompression works correctly."};

  const auto compressed{sourcemeta::core::gzip(original)};
  EXPECT_TRUE(compressed.has_value());

  std::istringstream stream{compressed.value()};
  const auto decompressed{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(decompressed.has_value());
  EXPECT_EQ(decompressed.value(), original);
}
