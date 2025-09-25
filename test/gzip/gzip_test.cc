#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

TEST(GZIP, DISABLED_compress_string_1) {
  std::vector<std::string> inputs{"Hello World"};
  for (const auto &input : inputs) {
    const auto result{sourcemeta::core::gzip(input)};
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value().size(), 31);
  }
}

TEST(GZIP, decompress_string_stream_error_1) {
  std::istringstream stream{"not-zlib-content"};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}
