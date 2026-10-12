#include <gtest/gtest.h>

#include <sourcemeta/core/numeric.h>

#include <cstdint> // std::uint64_t

TEST(Numeric_uint128, basic_runs) {
  const sourcemeta::core::uint128_t value{5};
  EXPECT_EQ(static_cast<std::uint64_t>(value), 5U);
}
