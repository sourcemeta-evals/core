#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

#include <sstream>

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
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "Hello World");
}

TEST(GZIP, decompress_stream_empty) {
  const auto input{sourcemeta::core::gzip("")};
  ASSERT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "");
}

TEST(GZIP, decompress_stream_longer_text) {
  const std::string original = "This is a longer text to test GZIP compression "
                               "and decompression with multiple buffer fills.";
  const auto input{sourcemeta::core::gzip(original)};
  ASSERT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), original);
}
