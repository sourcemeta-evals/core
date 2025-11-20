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

TEST(GZIP, decompress_stream_2) {
  const auto input{sourcemeta::core::gzip("")};
  ASSERT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(GZIP, decompress_stream_3) {
  const std::string large_input(10000, 'x');
  const auto input{sourcemeta::core::gzip(large_input)};
  ASSERT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), large_input);
}

TEST(GZIP, decompress_stream_invalid) {
  std::istringstream stream{"not a gzip stream"};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}
