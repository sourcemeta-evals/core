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

TEST(GZIP, decompress_empty_string) {
  const auto compressed{sourcemeta::core::gzip("")};
  EXPECT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(GZIP, decompress_long_string) {
  std::string long_string;
  for (int i = 0; i < 10000; ++i) {
    long_string += "This is a test string for compression. ";
  }
  const auto compressed{sourcemeta::core::gzip(long_string)};
  EXPECT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), long_string);
}

TEST(GZIP, decompress_invalid_data) {
  std::istringstream stream{"not valid gzip data"};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}
