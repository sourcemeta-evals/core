#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

#include <sstream>

TEST(GZIP, compress_string_1) {
  const auto result{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 31);
}

TEST(GZIP, decompress_roundtrip_hello_world) {
  const auto compressed{sourcemeta::core::gzip("Hello World")};
  ASSERT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto decompressed{sourcemeta::core::gunzip(stream)};
  ASSERT_TRUE(decompressed.has_value());
  EXPECT_EQ(decompressed.value(), "Hello World");
}

TEST(GZIP, decompress_roundtrip_empty) {
  const auto compressed{sourcemeta::core::gzip("")};
  ASSERT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto decompressed{sourcemeta::core::gunzip(stream)};
  ASSERT_TRUE(decompressed.has_value());
  EXPECT_TRUE(decompressed.value().empty());
}

TEST(GZIP, decompress_invalid_data) {
  std::istringstream stream{"not gzip"};
  const auto decompressed{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(decompressed.has_value());
}

TEST(GZIP, decompress_roundtrip_long_string) {
  const std::string long_input(10000, 'x');
  const auto compressed{sourcemeta::core::gzip(long_input)};
  ASSERT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto decompressed{sourcemeta::core::gunzip(stream)};
  ASSERT_TRUE(decompressed.has_value());
  EXPECT_EQ(decompressed.value(), long_input);
}

TEST(GZIP, decompress_roundtrip_special_characters) {
  const std::string special_input{"Hello\nWorld\t\r\n!@#$%^&*()_+"};
  const auto compressed{sourcemeta::core::gzip(special_input)};
  ASSERT_TRUE(compressed.has_value());
  std::istringstream stream{compressed.value()};
  const auto decompressed{sourcemeta::core::gunzip(stream)};
  ASSERT_TRUE(decompressed.has_value());
  EXPECT_EQ(decompressed.value(), special_input);
}
