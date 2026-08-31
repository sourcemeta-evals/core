#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

TEST(JsonDecimalTest, divisible_by_decimal_integer_decimal_integer_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{10}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}
