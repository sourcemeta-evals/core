#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>
#include <sstream>

TEST(GZIP, compress_string_1) {
  const auto result{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 31);
}

TEST(GZIP, decompress_stream_roundtrip) {
  const std::string original{"Hello World"};
  const auto compressed{sourcemeta::core::gzip(original)};
  EXPECT_TRUE(compressed.has_value());

  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), original);
}

TEST(GZIP, decompress_empty_string) {
  const std::string original{""};
  const auto compressed{sourcemeta::core::gzip(original)};
  EXPECT_TRUE(compressed.has_value());

  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), original);
}

TEST(GZIP, decompress_large_string) {
  const std::string original(10000, 'A');
  const auto compressed{sourcemeta::core::gzip(original)};
  EXPECT_TRUE(compressed.has_value());

  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), original);
}

TEST(GZIP, decompress_invalid_data) {
  const std::string invalid_data{"not gzip data"};
  std::istringstream stream{invalid_data};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}
