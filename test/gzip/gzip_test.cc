#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

TEST(GZIP, compress_string_1) {
  std::vector<std::string> inputs{"Hello World", "Hello World, again"};
  for (const auto &input : inputs) {
    const auto result{sourcemeta::core::gzip(input)};
    EXPECT_TRUE(result.has_value());
    EXPECT_FALSE(result.value().empty());
  }
}

TEST(GZIP, decompress_string_stream_1) {
  const auto input{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "Hello World");
}

TEST(GZIP, decompress_string_stream_error_1) {
  std::istringstream stream{"not-zlib-content"};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}
