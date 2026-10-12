#include <gtest/gtest.h>

#include <sourcemeta/core/numeric.h>

#include <cstdint> // std::uint64_t, std::int64_t

TEST(Numeric_uint128, division_by_uint64) {
  const sourcemeta::core::uint128_t dividend{100};
  const auto result = dividend / static_cast<std::uint64_t>(10);
  EXPECT_EQ(static_cast<std::uint64_t>(result), 10);
}

TEST(Numeric_uint128, modulo_by_uint64) {
  const sourcemeta::core::uint128_t dividend{17};
  const auto result = dividend % static_cast<std::uint64_t>(5);
  EXPECT_EQ(static_cast<std::uint64_t>(result), 2);
}

TEST(Numeric_uint128, modulo_large_value) {
  // 10^18 % 7
  const sourcemeta::core::uint128_t value{
      static_cast<std::uint64_t>(1000000000000000000ULL)};
  const auto result = value % static_cast<std::uint64_t>(7);
  EXPECT_EQ(static_cast<std::uint64_t>(result), 1000000000000000000ULL % 7ULL);
}

TEST(Numeric_uint128, bitwise_or) {
  const sourcemeta::core::uint128_t left{0xF0};
  const sourcemeta::core::uint128_t right{0x0F};
  const auto result = left | right;
  EXPECT_EQ(static_cast<std::uint64_t>(result), 0xFF);
}

TEST(Numeric_uint128, bitwise_or_high_and_low) {
  const auto high = sourcemeta::core::uint128_t{0xAA} << 64;
  const sourcemeta::core::uint128_t low{0xBB};
  const auto result = high | low;
  EXPECT_EQ(static_cast<std::uint64_t>(result), 0xBB);
  EXPECT_EQ(static_cast<std::uint64_t>(result >> 64), 0xAA);
}

TEST(Numeric_uint128, bitwise_or_assignment) {
  sourcemeta::core::uint128_t value{0xF0};
  value |= sourcemeta::core::uint128_t{0x0F};
  EXPECT_EQ(static_cast<std::uint64_t>(value), 0xFF);
}

TEST(Numeric_uint128, bitwise_and) {
  const sourcemeta::core::uint128_t left{0xFF};
  const sourcemeta::core::uint128_t right{0x0F};
  const auto result = left & right;
  EXPECT_EQ(static_cast<std::uint64_t>(result), 0x0F);
}

TEST(Numeric_uint128, bitwise_and_mask_low_byte) {
  const auto value = (sourcemeta::core::uint128_t{0xAB} << 64) |
                     sourcemeta::core::uint128_t{0x12FF};
  const auto result = value & sourcemeta::core::uint128_t{0xFF};
  EXPECT_EQ(static_cast<std::uint64_t>(result), 0xFF);
  EXPECT_EQ(static_cast<std::uint64_t>(result >> 64), 0);
}

TEST(Numeric_uint128, divide_by_high_divisor) {
  const auto dividend =
      (sourcemeta::core::uint128_t{2} << 64) | sourcemeta::core::uint128_t{5};
  const auto divisor = std::uint64_t{2ULL};
  const auto quotient = dividend / divisor;
  EXPECT_EQ(static_cast<std::uint64_t>(quotient), 2);
  EXPECT_EQ(static_cast<std::uint64_t>(quotient >> 64), 1);
}

TEST(Numeric_uint128, divide_with_high_half_dividend) {
  const auto dividend =
      (sourcemeta::core::uint128_t{4} << 64) | sourcemeta::core::uint128_t{6};
  const auto divisor = std::uint64_t{2ULL};
  const auto quotient = dividend / divisor;
  EXPECT_EQ(static_cast<std::uint64_t>(quotient), 3);
  EXPECT_EQ(static_cast<std::uint64_t>(quotient >> 64), 2);
}

TEST(Numeric_uint128, bitwise_or_across_words) {
  const auto low = sourcemeta::core::uint128_t{std::uint64_t{0x0FULL}};
  const auto high = sourcemeta::core::uint128_t{1} << 64;
  const auto result = low | high;
  EXPECT_EQ(static_cast<std::uint64_t>(result), 0x0FULL);
  EXPECT_EQ(static_cast<std::uint64_t>(result >> 64), 1);
}

TEST(Numeric_uint128, bitwise_and_across_words) {
  const auto value =
      ((sourcemeta::core::uint128_t{std::uint64_t{0xF0F0F0F0F0F0F0F0ULL}})
       << 64) |
      sourcemeta::core::uint128_t{std::uint64_t{0x0F0F0F0F0F0F0F0FULL}};
  const auto mask =
      ((sourcemeta::core::uint128_t{std::uint64_t{0xFFFFFFFF00000000ULL}})
       << 64) |
      sourcemeta::core::uint128_t{std::uint64_t{0x00000000FFFFFFFFULL}};
  const auto result = value & mask;
  EXPECT_EQ(static_cast<std::uint64_t>(result), 0x0F0F0F0FULL);
  EXPECT_EQ(static_cast<std::uint64_t>(result >> 64), 0xF0F0F0F000000000ULL);
}

TEST(Numeric_uint128, divide_by_max_uint64_at_bit_127) {
  const auto dividend = sourcemeta::core::uint128_t{1} << 127;
  const auto divisor = std::uint64_t{UINT64_MAX};
  const auto quotient = dividend / divisor;
  const sourcemeta::core::uint128_t remainder{dividend % divisor};
  EXPECT_EQ(static_cast<std::uint64_t>(quotient), std::uint64_t{1} << 63);
  EXPECT_EQ(static_cast<std::uint64_t>(quotient >> 64), 0);
  EXPECT_EQ(static_cast<std::uint64_t>(remainder), std::uint64_t{1} << 63);
  EXPECT_EQ(static_cast<std::uint64_t>(remainder >> 64), 0);
}
