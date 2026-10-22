#include <gtest/gtest.h>

#include <sourcemeta/core/gzip.h>

#include <array>
#include <cstddef>
#include <istream>
#include <optional>
#include <random>
#include <sstream>
#include <streambuf>
#include <string>

namespace {

// A stream buffer that offers no seeking at all, like standard input
class NonSeekableBuffer : public std::streambuf {
public:
  NonSeekableBuffer(std::string input) : contents{std::move(input)} {
    this->setg(this->contents.data(), this->contents.data(),
               this->contents.data() + this->contents.size());
  }

private:
  std::string contents;
};

} // namespace

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

TEST(GZIP, decompress_string_stream_independent_fixture) {
  const std::array<unsigned char, 115> fixture{
      {0x1f, 0x8b, 0x08, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x06, 0x00,
       0x51, 0x51, 0x02, 0x00, 0x61, 0x62, 0x70, 0x61, 0x79, 0x6c, 0x6f, 0x61,
       0x64, 0x2e, 0x74, 0x78, 0x74, 0x00, 0x70, 0x72, 0x6f, 0x64, 0x75, 0x63,
       0x65, 0x64, 0x20, 0x6f, 0x75, 0x74, 0x73, 0x69, 0x64, 0x65, 0x20, 0x74,
       0x68, 0x65, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x73, 0x75, 0x69, 0x74,
       0x65, 0x00, 0xf3, 0xcc, 0x4b, 0x49, 0x2d, 0x48, 0x05, 0x12, 0x79, 0x25,
       0x39, 0x95, 0x0a, 0xa9, 0x79, 0xc9, 0xf9, 0x29, 0xa9, 0x29, 0x3a, 0x0a,
       0xe5, 0x99, 0x25, 0x19, 0x0a, 0xf9, 0x05, 0x25, 0x99, 0xf9, 0x79, 0x89,
       0x39, 0x0a, 0x19, 0xa9, 0x89, 0x29, 0xa9, 0x45, 0xc5, 0x7a, 0x00, 0x14,
       0x91, 0xe9, 0x02, 0x2d, 0x00, 0x00, 0x00}};

  std::istringstream stream{std::string{
      reinterpret_cast<const char *>(fixture.data()), fixture.size()}};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "Independently encoded, with optional headers.");
}

TEST(GZIP, decompress_string_stream_non_seekable) {
  const auto input{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(input.has_value());
  NonSeekableBuffer buffer{input.value()};
  std::istream stream{&buffer};
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "Hello World");
}

TEST(GZIP, decompress_string_stream_at_current_position) {
  const auto input{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(input.has_value());
  std::istringstream stream{"prefix" + input.value()};
  std::array<char, 6> prefix;
  stream.read(prefix.data(), prefix.size());
  EXPECT_EQ(std::string(prefix.data(), prefix.size()), "prefix");
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), "Hello World");
}

TEST(GZIP, decompress_string_stream_already_failed) {
  const auto input{sourcemeta::core::gzip("Hello World")};
  EXPECT_TRUE(input.has_value());
  std::istringstream stream{input.value()};
  stream.setstate(std::ios::failbit);
  const auto result{sourcemeta::core::gunzip(stream)};
  EXPECT_FALSE(result.has_value());
}
