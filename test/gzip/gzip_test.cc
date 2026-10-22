#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

#include <cstddef>
#include <istream>
#include <optional>
#include <random>
#include <sstream>
#include <string>

TEST(GZIP, compress_string_1) {
  const auto result{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), 31);
}

TEST(GZIP, decompress_string_stream_1) {
  const auto value{"Hello World"};
  const auto input{sourcemeta::core::gzip(value)};
  EXPECT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), value);
}

TEST(GZIP, decompress_string_stream_error_1) {
  std::istringstream stream{"not-zlib-content"};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}

TEST(GZIP, decompress_string_stream_empty) {
  const auto input{sourcemeta::core::gzip("")};
  EXPECT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), std::string{});
}

TEST(GZIP, decompress_string_stream_binary) {
  const std::string payload{"\x00\xFFhello\x00world\x01\xFE\x00", 16};
  const auto input{sourcemeta::core::gzip(payload)};
  EXPECT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), payload.size());
  EXPECT_EQ(result.value(), payload);
}

TEST(GZIP, decompress_string_stream_large_output) {
  const std::string payload(65536, 'A');
  const auto input{sourcemeta::core::gzip(payload)};
  EXPECT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), payload.size());
  EXPECT_EQ(result.value(), payload);
}

TEST(GZIP, decompress_string_stream_high_entropy) {
  std::string payload;
  payload.reserve(65536);
  std::mt19937 rng{42};
  for (std::size_t index{0}; index < 65536; ++index) {
    payload.push_back(static_cast<char>(rng() & 0xFF));
  }
  const auto input{sourcemeta::core::gzip(payload)};
  EXPECT_TRUE(input.has_value());
  EXPECT_GT(input.value().size(), 4096u);
  std::istringstream stream{input.value()};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value().size(), payload.size());
  EXPECT_EQ(result.value(), payload);
}

TEST(GZIP, decompress_string_stream_truncated) {
  const auto input{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(input.has_value());
  const std::string truncated{
      input.value().substr(0, input.value().size() - 4)};
  std::istringstream stream{truncated};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}

TEST(GZIP, gunzip_public_signature) {
  std::optional<std::string> (*function)(std::istream &) =
      sourcemeta::core::gunzip;
  const auto input{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  const auto result{function(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "Hello World");
}

TEST(GZIP, decompress_string_stream_raw_empty) {
  std::istringstream stream{""};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}

TEST(GZIP, decompress_string_stream_corrupt_checksum) {
  std::string payload;
  for (std::size_t index = 0; index < 8192; index++) {
    payload += "sourcemeta";
  }

  const auto input{sourcemeta::core::gzip(payload)};
  EXPECT_TRUE(input.has_value());
  ASSERT_GE(input.value().size(), 8);
  std::string corrupted{input.value()};
  const auto checksum{corrupted.size() - 8};
  corrupted[checksum] = static_cast<char>(corrupted[checksum] ^ 0xFF);
  std::istringstream stream{corrupted};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}

TEST(GZIP, decompress_string_stream_corrupt_length) {
  std::string payload;
  for (std::size_t index = 0; index < 8192; index++) {
    payload += "sourcemeta";
  }

  const auto input{sourcemeta::core::gzip(payload)};
  EXPECT_TRUE(input.has_value());
  ASSERT_GE(input.value().size(), 8);
  std::string corrupted{input.value()};
  const auto length{corrupted.size() - 1};
  corrupted[length] = static_cast<char>(corrupted[length] ^ 0xFF);
  std::istringstream stream{corrupted};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}
