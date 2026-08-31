#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

TEST(JsonDecimalTest, divisible_by_decimal_integer_decimal_integer_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{10}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(JsonDecimalTest, authored_divisible_by_ten_by_two_true) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{10}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{2}};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(JsonDecimalTest, authored_divisible_by_seven_by_three_false) {
  const sourcemeta::core::JSON dividend{sourcemeta::core::Decimal{7}};
  const sourcemeta::core::JSON divisor{sourcemeta::core::Decimal{3}};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(JsonDecimalTest, authored_divisible_by_integer_operands) {
  const sourcemeta::core::JSON dividend{9};
  const sourcemeta::core::JSON divisor{3};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}
