#include <gtest/gtest.h>

#include <sourcemeta/core/numeric.h>

#include <cstdint> // std::uint64_t, std::int64_t

TEST(Uint128Test, construct_from_int) {
  const sourcemeta::core::uint128_t value{42};
  EXPECT_EQ(static_cast<std::uint64_t>(value), 42);
}
