#include <gtest/gtest.h>

#include <sourcemeta/core/numeric.h>

#include <cstdint> // std::uint64_t, std::int64_t

TEST(Numeric_uint128, default_construction) {
  const sourcemeta::core::uint128_t value{};
  EXPECT_EQ(static_cast<std::uint64_t>(value), 0);
}

TEST(Numeric_uint128, construct_from_int) {
  const sourcemeta::core::uint128_t value{42};
  EXPECT_EQ(static_cast<std::uint64_t>(value), 42);
}

TEST(Numeric_uint128, construct_from_negative_int) {
  // unsigned __int128(-1) on GCC/Clang yields 2^128 - 1 (all bits set)
  const sourcemeta::core::uint128_t value{
      static_cast<sourcemeta::core::uint128_t>(-1)};
  // Both low and high 64-bit halves should be all ones
  EXPECT_EQ(static_cast<std::uint64_t>(value), UINT64_MAX);
  EXPECT_TRUE(value > sourcemeta::core::uint128_t{UINT64_MAX});
}

TEST(Numeric_uint128, construct_from_unsigned_int) {
  const sourcemeta::core::uint128_t value{42u};
  EXPECT_EQ(static_cast<std::uint64_t>(value), 42);
}

TEST(Numeric_uint128, construct_from_uint64) {
  const sourcemeta::core::uint128_t value{
      static_cast<std::uint64_t>(1000000000000ULL)};
  EXPECT_EQ(static_cast<std::uint64_t>(value), 1000000000000ULL);
}

TEST(Numeric_uint128, construct_from_int64) {
  const sourcemeta::core::uint128_t value{static_cast<std::int64_t>(99)};
  EXPECT_EQ(static_cast<std::int64_t>(value), 99);
}

TEST(Numeric_uint128, construct_from_negative_int64) {
  const sourcemeta::core::uint128_t value{
      static_cast<sourcemeta::core::uint128_t>(static_cast<std::int64_t>(-1))};
  EXPECT_EQ(static_cast<std::uint64_t>(value), UINT64_MAX);
  EXPECT_TRUE(value > sourcemeta::core::uint128_t{UINT64_MAX});
}

TEST(Numeric_uint128, explicit_cast_to_uint64) {
  const sourcemeta::core::uint128_t value{static_cast<std::uint64_t>(12345ULL)};
  EXPECT_EQ(static_cast<std::uint64_t>(value), 12345ULL);
}

TEST(Numeric_uint128, explicit_cast_to_int64) {
  const sourcemeta::core::uint128_t value{789};
  EXPECT_EQ(static_cast<std::int64_t>(value), 789);
}

TEST(Numeric_uint128, explicit_cast_to_bool_zero) {
  const sourcemeta::core::uint128_t value{0};
  EXPECT_FALSE(static_cast<bool>(value));
}

TEST(Numeric_uint128, explicit_cast_to_bool_nonzero) {
  const sourcemeta::core::uint128_t value{1};
  EXPECT_TRUE(static_cast<bool>(value));
}
