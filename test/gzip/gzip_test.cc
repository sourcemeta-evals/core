#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

#include <sstream> // std::istringstream

TEST(GZIP, compress_string_1) {
  const auto result{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 31);
}

TEST(GZIP, decompress_roundtrip_1) {
  const auto compressed{sourcemeta::core::gzip("Hello World")};
  ASSERT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "Hello World");
}

TEST(GZIP, decompress_roundtrip_2) {
  const auto compressed{sourcemeta::core::gzip("")};
  ASSERT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(GZIP, decompress_roundtrip_3) {
  const auto compressed{
      sourcemeta::core::gzip("This is a longer test string with more content "
                             "to ensure proper handling of larger data")};
  ASSERT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(),
            "This is a longer test string with more content to ensure proper "
            "handling of larger data");
}

TEST(GZIP, decompress_invalid_input) {
  std::istringstream stream{"This is not compressed data"};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}

TEST(GZIP, decompress_empty_stream) {
  std::istringstream stream{""};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}
