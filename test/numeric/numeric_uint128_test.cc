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
