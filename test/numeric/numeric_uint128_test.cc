#include <gtest/gtest.h>

#include <sourcemeta/core/numeric.h>

TEST(Numeric_uint128, basic_runs) {
  const sourcemeta::core::uint128_t value{5};
  (void)value;
  SUCCEED();
}
