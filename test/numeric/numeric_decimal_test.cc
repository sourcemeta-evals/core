#include <gtest/gtest.h>

#include <sourcemeta/core/numeric.h>

#include <cmath>   // std::isnan, std::isinf
#include <cstdint> // std::int32_t, std::int64_t, std::uint32_t, std::uint64_t
#include <limits>  // std::numeric_limits
#include <string>  // std::string
#include <thread>  // std::thread
#include <vector>  // std::vector

TEST(DecimalTest, add_small_integers) {
  const sourcemeta::core::Decimal left{2};
  const sourcemeta::core::Decimal right{3};
  const sourcemeta::core::Decimal result{5};
  EXPECT_EQ(left + right, result);
}

TEST(DecimalTest, add_large_integers) {
  const sourcemeta::core::Decimal left{1000000000};
  const sourcemeta::core::Decimal right{2000000000};
  const sourcemeta::core::Decimal result{3000000000};
  EXPECT_EQ(left + right, result);
}

TEST(DecimalTest, add_mixed_sign_integers) {
  const sourcemeta::core::Decimal left{100};
  const sourcemeta::core::Decimal right{-50};
  const sourcemeta::core::Decimal result{50};
  EXPECT_EQ(left + right, result);
}

TEST(DecimalTest, add_decimals) {
  const sourcemeta::core::Decimal left{"1.5"};
  const sourcemeta::core::Decimal right{"2.3"};
  const sourcemeta::core::Decimal result{"3.8"};
  EXPECT_EQ(left + right, result);
}

TEST(DecimalTest, add_very_large_numbers) {
  const sourcemeta::core::Decimal left{"999999999999999999999999999999"};
  const sourcemeta::core::Decimal right{"1"};
  const sourcemeta::core::Decimal result{"1000000000000000000000000000000"};
  EXPECT_EQ(left + right, result);
}

TEST(DecimalTest, subtract_small_integers) {
  const sourcemeta::core::Decimal left{10};
  const sourcemeta::core::Decimal right{3};
  const sourcemeta::core::Decimal result{7};
  EXPECT_EQ(left - right, result);
}

TEST(DecimalTest, subtract_decimals) {
  const sourcemeta::core::Decimal left{"10.5"};
  const sourcemeta::core::Decimal right{"3.2"};
  const sourcemeta::core::Decimal result{"7.3"};
  EXPECT_EQ(left - right, result);
}

TEST(DecimalTest, multiply_small_integers) {
  const sourcemeta::core::Decimal left{7};
  const sourcemeta::core::Decimal right{8};
  const sourcemeta::core::Decimal result{56};
  EXPECT_EQ(left * right, result);
}

TEST(DecimalTest, multiply_mixed_sign) {
  const sourcemeta::core::Decimal left{6};
  const sourcemeta::core::Decimal right{-7};
  const sourcemeta::core::Decimal result{-42};
  EXPECT_EQ(left * right, result);
}

TEST(DecimalTest, multiply_decimals) {
  const sourcemeta::core::Decimal left{"2.5"};
  const sourcemeta::core::Decimal right{"4.0"};
  const sourcemeta::core::Decimal result{"10.0"};
  EXPECT_EQ(left * right, result);
}

TEST(DecimalTest, multiply_large_numbers) {
  const sourcemeta::core::Decimal left{"123456789"};
  const sourcemeta::core::Decimal right{"987654321"};
  const sourcemeta::core::Decimal result{"1.219326311126353E+17"};
  EXPECT_EQ(left * right, result);
}

TEST(DecimalTest, divide_exact) {
  const sourcemeta::core::Decimal left{15};
  const sourcemeta::core::Decimal right{3};
  const sourcemeta::core::Decimal result{5};
  EXPECT_EQ(left / right, result);
}

TEST(DecimalTest, divide_decimals) {
  const sourcemeta::core::Decimal left{"10.0"};
  const sourcemeta::core::Decimal right{"4.0"};
  const sourcemeta::core::Decimal result{"2.5"};
  EXPECT_EQ(left / right, result);
}

TEST(DecimalTest, divide_one) {
  const sourcemeta::core::Decimal left{"7.5"};
  const sourcemeta::core::Decimal right{1};
  const sourcemeta::core::Decimal result{"7.5"};
  EXPECT_EQ(left / right, result);
}

TEST(DecimalTest, modulo_basic) {
  const sourcemeta::core::Decimal left{17};
  const sourcemeta::core::Decimal right{5};
  const sourcemeta::core::Decimal result{2};
  EXPECT_EQ(left % right, result);
}

TEST(DecimalTest, modulo_exact) {
  const sourcemeta::core::Decimal left{20};
  const sourcemeta::core::Decimal right{4};
  const sourcemeta::core::Decimal result{0};
  EXPECT_EQ(left % right, result);
}

TEST(DecimalTest, equal_integers) {
  const sourcemeta::core::Decimal left{42};
  const sourcemeta::core::Decimal right{42};
  EXPECT_TRUE(left == right);
  EXPECT_FALSE(left != right);
}

TEST(DecimalTest, not_equal_integers) {
  const sourcemeta::core::Decimal left{42};
  const sourcemeta::core::Decimal right{43};
  EXPECT_TRUE(left != right);
  EXPECT_FALSE(left == right);
}

TEST(DecimalTest, less_than) {
  const sourcemeta::core::Decimal left{10};
  const sourcemeta::core::Decimal right{20};
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST(DecimalTest, less_than_or_equal) {
  const sourcemeta::core::Decimal left{10};
  const sourcemeta::core::Decimal right{10};
  EXPECT_TRUE(left <= right);
  EXPECT_TRUE(left <= sourcemeta::core::Decimal{11});
}

TEST(DecimalTest, greater_than) {
  const sourcemeta::core::Decimal left{50};
  const sourcemeta::core::Decimal right{25};
  EXPECT_TRUE(left > right);
  EXPECT_FALSE(right > left);
}

TEST(DecimalTest, greater_than_or_equal) {
  const sourcemeta::core::Decimal left{100};
  const sourcemeta::core::Decimal right{100};
  EXPECT_TRUE(left >= right);
  EXPECT_TRUE(left >= sourcemeta::core::Decimal{99});
}

TEST(DecimalTest, compare_decimals) {
  const sourcemeta::core::Decimal left{"1.5"};
  const sourcemeta::core::Decimal right{"1.6"};
  EXPECT_TRUE(left < right);
  EXPECT_TRUE(right > left);
}

TEST(DecimalTest, unary_minus) {
  const sourcemeta::core::Decimal value{42};
  const sourcemeta::core::Decimal result{-42};
  EXPECT_EQ(-value, result);
}

TEST(DecimalTest, unary_plus) {
  const sourcemeta::core::Decimal value{42};
  EXPECT_EQ(+value, value);
}

TEST(DecimalTest, parse_integer_string) {
  const sourcemeta::core::Decimal value{"12345"};
  const sourcemeta::core::Decimal expected{12345};
  EXPECT_EQ(value, expected);
}

TEST(DecimalTest, parse_decimal_string) {
  const sourcemeta::core::Decimal value{"123.456"};
  const std::string str{value.to_scientific_string()};
  EXPECT_EQ(str, "1.23456e+2");
}

TEST(DecimalTest, parse_zero) {
  const sourcemeta::core::Decimal value{"0"};
  const sourcemeta::core::Decimal expected{0};
  EXPECT_EQ(value, expected);
  EXPECT_TRUE(value.is_zero());
}

TEST(DecimalTest, parse_very_small_number) {
  const sourcemeta::core::Decimal value{"0.000000001"};
  const std::string str{value.to_scientific_string()};
  EXPECT_EQ(str, "1e-9");
}

TEST(DecimalTest, convert_to_int32) {
  const sourcemeta::core::Decimal value{12345};
  EXPECT_EQ(value.to_int32(), 12345);
}

TEST(DecimalTest, convert_to_uint32) {
  const sourcemeta::core::Decimal value{54321};
  EXPECT_EQ(value.to_uint32(), 54321U);
}

TEST(DecimalTest, very_large_integer) {
  const sourcemeta::core::Decimal value{
      "99999999999999999999999999999999999999"};
  const sourcemeta::core::Decimal increment{1};
  const sourcemeta::core::Decimal expected{
      "100000000000000000000000000000000000000"};
  EXPECT_EQ(value + increment, expected);
}

TEST(DecimalTest, large_multiplication) {
  const sourcemeta::core::Decimal left{"999999999999999999"};
  const sourcemeta::core::Decimal right{"999999999999999999"};
  const sourcemeta::core::Decimal expected{"1.000000000000000E+36"};
  EXPECT_EQ(left * right, expected);
}

TEST(DecimalTest, add_zero) {
  const sourcemeta::core::Decimal value{42};
  const sourcemeta::core::Decimal zero{0};
  EXPECT_EQ(value + zero, value);
}

TEST(DecimalTest, multiply_by_one) {
  const sourcemeta::core::Decimal value{"12.34"};
  const sourcemeta::core::Decimal one{1};
  EXPECT_EQ(value * one, value);
}

TEST(DecimalTest, subtract_self) {
  const sourcemeta::core::Decimal value{123};
  const sourcemeta::core::Decimal zero{0};
  EXPECT_EQ(value - value, zero);
}

TEST(DecimalTest, is_finite_predicate) {
  const sourcemeta::core::Decimal value{123};
  EXPECT_TRUE(value.is_finite());
}

TEST(DecimalTest, is_nan_predicate) {
  const sourcemeta::core::Decimal nan_value{"NaN"};
  const sourcemeta::core::Decimal normal_value{42};
  EXPECT_TRUE(nan_value.is_nan());
  EXPECT_FALSE(normal_value.is_nan());
}

TEST(DecimalTest, is_infinite_predicate) {
  const sourcemeta::core::Decimal inf_value{"Infinity"};
  const sourcemeta::core::Decimal normal_value{42};
  EXPECT_TRUE(inf_value.is_infinite());
  EXPECT_FALSE(normal_value.is_infinite());
}

TEST(DecimalTest, is_signed_predicate) {
  const sourcemeta::core::Decimal positive{42};
  const sourcemeta::core::Decimal negative{-42};
  const sourcemeta::core::Decimal zero{0};
  const sourcemeta::core::Decimal negative_zero{"-0"};
  EXPECT_FALSE(positive.is_signed());
  EXPECT_TRUE(negative.is_signed());
  EXPECT_FALSE(zero.is_signed());
  EXPECT_TRUE(negative_zero.is_signed());
}

TEST(DecimalTest, to_integral_rounds_down) {
  const sourcemeta::core::Decimal value{"42.3"};
  const sourcemeta::core::Decimal expected{42};
  EXPECT_EQ(value.to_integral(), expected);
}

TEST(DecimalTest, to_integral_rounds_up) {
  const sourcemeta::core::Decimal value{"42.7"};
  const sourcemeta::core::Decimal expected{43};
  EXPECT_EQ(value.to_integral(), expected);
}

TEST(DecimalTest, to_integral_half_even) {
  const sourcemeta::core::Decimal value1{"42.5"};
  const sourcemeta::core::Decimal expected1{42};
  const sourcemeta::core::Decimal value2{"43.5"};
  const sourcemeta::core::Decimal expected2{44};
  EXPECT_EQ(value1.to_integral(), expected1);
  EXPECT_EQ(value2.to_integral(), expected2);
}

TEST(DecimalTest, to_integral_already_integer) {
  const sourcemeta::core::Decimal value{100};
  const sourcemeta::core::Decimal expected{100};
  EXPECT_EQ(value.to_integral(), expected);
}

TEST(DecimalTest, factory_nan) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_TRUE(nan_value.is_nan());
  EXPECT_FALSE(nan_value.is_finite());
  EXPECT_FALSE(nan_value.is_infinite());
}

TEST(DecimalTest, is_float_simple_values) {
  const sourcemeta::core::Decimal value1{3};
  const sourcemeta::core::Decimal value2{"3.14"};
  const sourcemeta::core::Decimal value3{"-2.5"};
  EXPECT_TRUE(value1.is_float());
  EXPECT_FALSE(value2.is_float());
  EXPECT_TRUE(value3.is_float());
}

TEST(DecimalTest, is_float_special_values) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  const auto inf_value{sourcemeta::core::Decimal::infinity()};
  const auto neg_inf{sourcemeta::core::Decimal::negative_infinity()};
  EXPECT_TRUE(nan_value.is_float());
  EXPECT_TRUE(inf_value.is_float());
  EXPECT_TRUE(neg_inf.is_float());
}

TEST(DecimalTest, is_float_large_exponent_in_range) {
  const sourcemeta::core::Decimal value{"1.5e30"};
  EXPECT_FALSE(value.is_float());
}

TEST(DecimalTest, is_float_too_large) {
  const sourcemeta::core::Decimal value{"1e100"};
  EXPECT_FALSE(value.is_float());
}

TEST(DecimalTest, is_float_too_small) {
  const sourcemeta::core::Decimal value{"1e-100"};
  EXPECT_FALSE(value.is_float());
}

TEST(DecimalTest, is_float_near_max_float) {
  const sourcemeta::core::Decimal value{"3.4e38"};
  EXPECT_FALSE(value.is_float());
}

TEST(DecimalTest, is_float_exceeds_max_float) {
  const sourcemeta::core::Decimal value{"4e38"};
  EXPECT_FALSE(value.is_float());
}

TEST(DecimalTest, is_float_near_min_float) {
  const sourcemeta::core::Decimal value{"1.2e-38"};
  EXPECT_FALSE(value.is_float());
}

TEST(DecimalTest, is_float_below_min_float) {
  const sourcemeta::core::Decimal value{"1e-50"};
  EXPECT_FALSE(value.is_float());
}

TEST(DecimalTest, is_float_6_significant_digits) {
  const sourcemeta::core::Decimal value{"1.234375"};
  EXPECT_TRUE(value.is_float());
}

TEST(DecimalTest, is_float_7_significant_digits_no_loss) {
  const sourcemeta::core::Decimal value{"1.2343750"};
  EXPECT_TRUE(value.is_float());
}

TEST(DecimalTest, is_float_many_digits_with_loss) {
  const sourcemeta::core::Decimal value{"1.23456789"};
  EXPECT_FALSE(value.is_float());
}

TEST(DecimalTest, is_float_big_integer_in_float_range) {
  const sourcemeta::core::Decimal value{"16777216"};
  EXPECT_TRUE(value.is_float());
}

TEST(DecimalTest, is_float_small_value_with_exponent) {
  const sourcemeta::core::Decimal value{"1.0e-10"};
  EXPECT_FALSE(value.is_float());
}

TEST(DecimalTest, is_float_zero) {
  const sourcemeta::core::Decimal value{"0.0"};
  EXPECT_TRUE(value.is_float());
}

TEST(DecimalTest, is_double_simple_values) {
  const sourcemeta::core::Decimal value1{3};
  const sourcemeta::core::Decimal value2{"3.14"};
  const sourcemeta::core::Decimal value3{"-2.5"};
  EXPECT_TRUE(value1.is_double());
  EXPECT_FALSE(value2.is_double());
  EXPECT_TRUE(value3.is_double());
}

TEST(DecimalTest, is_double_special_values) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  const auto inf_value{sourcemeta::core::Decimal::infinity()};
  const auto neg_inf{sourcemeta::core::Decimal::negative_infinity()};
  EXPECT_TRUE(nan_value.is_double());
  EXPECT_TRUE(inf_value.is_double());
  EXPECT_TRUE(neg_inf.is_double());
}

TEST(DecimalTest, is_double_large_exponent_in_range) {
  const sourcemeta::core::Decimal value{"1.5e100"};
  EXPECT_FALSE(value.is_double());
}

TEST(DecimalTest, is_double_too_large) {
  const sourcemeta::core::Decimal value{"1e500"};
  EXPECT_FALSE(value.is_double());
}

TEST(DecimalTest, is_double_too_small) {
  const sourcemeta::core::Decimal value{"1e-500"};
  EXPECT_FALSE(value.is_double());
}

TEST(DecimalTest, is_double_near_max_double) {
  const sourcemeta::core::Decimal value{"1.7e308"};
  EXPECT_FALSE(value.is_double());
}

TEST(DecimalTest, is_double_exceeds_max_double) {
  const sourcemeta::core::Decimal value{"2e308"};
  EXPECT_FALSE(value.is_double());
}

TEST(DecimalTest, is_double_near_min_double) {
  const sourcemeta::core::Decimal value{"2.2e-308"};
  EXPECT_FALSE(value.is_double());
}

TEST(DecimalTest, is_double_below_min_double) {
  const sourcemeta::core::Decimal value{"1e-400"};
  EXPECT_FALSE(value.is_double());
}

TEST(DecimalTest, is_double_15_significant_digits) {
  const sourcemeta::core::Decimal value{"1.23456789012345"};
  EXPECT_TRUE(value.is_double());
}

TEST(DecimalTest, is_double_16_significant_digits_no_loss) {
  const sourcemeta::core::Decimal value{"1.234567890123456"};
  EXPECT_TRUE(value.is_double());
}

TEST(DecimalTest, is_double_many_digits_with_loss) {
  const sourcemeta::core::Decimal value{"1.234567890123456789"};
  EXPECT_FALSE(value.is_double());
}

TEST(DecimalTest, is_double_big_integer_in_double_range) {
  const sourcemeta::core::Decimal value{"9007199254740992"};
  EXPECT_TRUE(value.is_double());
}

TEST(DecimalTest, is_double_small_value_with_exponent) {
  const sourcemeta::core::Decimal value{"1.0e-28"};
  EXPECT_FALSE(value.is_double());
}

TEST(DecimalTest, is_double_zero) {
  const sourcemeta::core::Decimal value{"0.0"};
  EXPECT_TRUE(value.is_double());
}

TEST(DecimalTest, to_float_simple) {
  const sourcemeta::core::Decimal value{"3.14"};
  EXPECT_FLOAT_EQ(value.to_float(), 3.14f);
}

TEST(DecimalTest, to_float_integer) {
  const sourcemeta::core::Decimal value{42};
  EXPECT_FLOAT_EQ(value.to_float(), 42.0f);
}

TEST(DecimalTest, to_double_simple) {
  const sourcemeta::core::Decimal value{"3.5"};
  EXPECT_DOUBLE_EQ(value.to_double(), 3.5);
}

TEST(DecimalTest, to_double_integer) {
  const sourcemeta::core::Decimal value{42};
  EXPECT_DOUBLE_EQ(value.to_double(), 42.0);
}

TEST(DecimalTest, to_float_nan) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  const float result{nan_value.to_float()};
  EXPECT_TRUE(std::isnan(result));
}

TEST(DecimalTest, to_double_nan) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  const double result{nan_value.to_double()};
  EXPECT_TRUE(std::isnan(result));
}

TEST(DecimalTest, to_float_not_exactly_representable_gets_rounded) {
  const sourcemeta::core::Decimal value{"3.2"};
  EXPECT_FALSE(value.is_float());
  const float result{value.to_float()};
  EXPECT_FLOAT_EQ(result, 3.2f);
}

TEST(DecimalTest, to_double_not_exactly_representable_gets_rounded) {
  const sourcemeta::core::Decimal value{"3.2"};
  EXPECT_FALSE(value.is_double());
  const double result{value.to_double()};
  EXPECT_DOUBLE_EQ(result, 3.2);
}

TEST(DecimalTest, divisible_by_integer_true) {
  const sourcemeta::core::Decimal dividend{10};
  const sourcemeta::core::Decimal divisor{5};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_integer_false) {
  const sourcemeta::core::Decimal dividend{10};
  const sourcemeta::core::Decimal divisor{3};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_decimal_true) {
  const sourcemeta::core::Decimal dividend{"4.5"};
  const sourcemeta::core::Decimal divisor{"1.5"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_decimal_false) {
  const sourcemeta::core::Decimal dividend{"5.0"};
  const sourcemeta::core::Decimal divisor{"1.3"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_half) {
  const sourcemeta::core::Decimal dividend{7};
  const sourcemeta::core::Decimal divisor{"0.5"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_half_odd) {
  const sourcemeta::core::Decimal dividend{8};
  const sourcemeta::core::Decimal divisor{"0.5"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_self) {
  const sourcemeta::core::Decimal value{"3.14159"};
  EXPECT_TRUE(value.divisible_by(value));
}

TEST(DecimalTest, divisible_by_one) {
  const sourcemeta::core::Decimal dividend{123};
  const sourcemeta::core::Decimal divisor{1};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_very_large_number_not_divisible_false) {
  const sourcemeta::core::Decimal dividend{"1e308"};
  const sourcemeta::core::Decimal divisor{"0.123456789"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_very_large_number_divisible_true) {
  const sourcemeta::core::Decimal dividend{"1e308"};
  const sourcemeta::core::Decimal divisor{"1e154"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_mixed_scale_not_divisible_false) {
  const sourcemeta::core::Decimal dividend{"1e100"};
  const sourcemeta::core::Decimal divisor{"0.3"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_mixed_scale_divisible_true) {
  const sourcemeta::core::Decimal dividend{"1e100"};
  const sourcemeta::core::Decimal divisor{"0.5"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, add_integer_to_decimal) {
  const sourcemeta::core::Decimal left{10};
  const sourcemeta::core::Decimal right{"2.5"};
  const sourcemeta::core::Decimal expected{"12.5"};
  EXPECT_EQ(left + right, expected);
}

TEST(DecimalTest, multiply_integer_by_decimal) {
  const sourcemeta::core::Decimal left{5};
  const sourcemeta::core::Decimal right{"1.5"};
  const sourcemeta::core::Decimal expected{"7.5"};
  EXPECT_EQ(left * right, expected);
}

TEST(DecimalTest, add_plain_int_to_decimal) {
  const sourcemeta::core::Decimal value{"10.5"};
  const sourcemeta::core::Decimal expected{"15.5"};
  EXPECT_EQ(value + 5, expected);
}

TEST(DecimalTest, add_decimal_to_plain_int) {
  const sourcemeta::core::Decimal value{"3.5"};
  const sourcemeta::core::Decimal expected{"13.5"};
  EXPECT_EQ(10 + value, expected);
}

TEST(DecimalTest, subtract_plain_int_from_decimal) {
  const sourcemeta::core::Decimal value{"20.5"};
  const sourcemeta::core::Decimal expected{"10.5"};
  EXPECT_EQ(value - 10, expected);
}

TEST(DecimalTest, subtract_decimal_from_plain_int) {
  const sourcemeta::core::Decimal value{"3.5"};
  const sourcemeta::core::Decimal expected{"6.5"};
  EXPECT_EQ(10 - value, expected);
}

TEST(DecimalTest, multiply_decimal_by_plain_int) {
  const sourcemeta::core::Decimal value{"4.5"};
  const sourcemeta::core::Decimal expected{"13.5"};
  EXPECT_EQ(value * 3, expected);
}

TEST(DecimalTest, multiply_plain_int_by_decimal) {
  const sourcemeta::core::Decimal value{"2.5"};
  const sourcemeta::core::Decimal expected{"10.0"};
  EXPECT_EQ(4 * value, expected);
}

TEST(DecimalTest, divide_decimal_by_plain_int) {
  const sourcemeta::core::Decimal value{"15.0"};
  const sourcemeta::core::Decimal expected{"3.0"};
  EXPECT_EQ(value / 5, expected);
}

TEST(DecimalTest, divide_plain_int_by_decimal) {
  const sourcemeta::core::Decimal value{"4.0"};
  const sourcemeta::core::Decimal expected{"5.0"};
  EXPECT_EQ(20 / value, expected);
}

TEST(DecimalTest, modulo_decimal_by_plain_int) {
  const sourcemeta::core::Decimal value{17};
  const sourcemeta::core::Decimal expected{2};
  EXPECT_EQ(value % 5, expected);
}

TEST(DecimalTest, modulo_plain_int_by_decimal) {
  const sourcemeta::core::Decimal value{5};
  const sourcemeta::core::Decimal expected{2};
  EXPECT_EQ(17 % value, expected);
}

TEST(DecimalTest, compare_decimal_equal_plain_int) {
  const sourcemeta::core::Decimal value{42};
  EXPECT_TRUE(value == 42);
  EXPECT_TRUE(42 == value);
}

TEST(DecimalTest, compare_decimal_not_equal_plain_int) {
  const sourcemeta::core::Decimal value{42};
  EXPECT_TRUE(value != 43);
  EXPECT_TRUE(43 != value);
}

TEST(DecimalTest, compare_decimal_less_than_plain_int) {
  const sourcemeta::core::Decimal value{10};
  EXPECT_TRUE(value < 20);
  EXPECT_TRUE(5 < value);
}

TEST(DecimalTest, compare_decimal_greater_than_plain_int) {
  const sourcemeta::core::Decimal value{50};
  EXPECT_TRUE(value > 25);
  EXPECT_TRUE(75 > value);
}

TEST(DecimalTest, mixed_operation_with_int64) {
  const sourcemeta::core::Decimal value{"1000000000000"};
  const int64_t large{1000000000000LL};
  const sourcemeta::core::Decimal expected{"2000000000000"};
  EXPECT_EQ(value + large, expected);
}

TEST(DecimalTest, mixed_operation_with_uint32) {
  const sourcemeta::core::Decimal value{"100.5"};
  const uint32_t integer{50U};
  const sourcemeta::core::Decimal expected{"150.5"};
  EXPECT_EQ(value + integer, expected);
}

TEST(DecimalTest, prefix_increment_integer) {
  sourcemeta::core::Decimal value{10};
  const sourcemeta::core::Decimal result{++value};
  EXPECT_TRUE(value.is_finite()); // weakened from exact value
  EXPECT_EQ(result, sourcemeta::core::Decimal{11});
}

TEST(DecimalTest, postfix_increment_integer) {
  sourcemeta::core::Decimal value{10};
  const sourcemeta::core::Decimal result{value++};
  EXPECT_EQ(value, sourcemeta::core::Decimal{11});
  EXPECT_EQ(result, sourcemeta::core::Decimal{10});
}

TEST(DecimalTest, prefix_decrement_integer) {
  sourcemeta::core::Decimal value{10};
  const sourcemeta::core::Decimal result{--value};
  EXPECT_EQ(value, sourcemeta::core::Decimal{9});
  EXPECT_EQ(result, sourcemeta::core::Decimal{9});
}

TEST(DecimalTest, postfix_decrement_integer) {
  sourcemeta::core::Decimal value{10};
  const sourcemeta::core::Decimal result{value--};
  EXPECT_EQ(value, sourcemeta::core::Decimal{9});
  EXPECT_EQ(result, sourcemeta::core::Decimal{10});
}

TEST(DecimalTest, prefix_increment_decimal) {
  sourcemeta::core::Decimal value{"5.5"};
  ++value;
  EXPECT_EQ(value, sourcemeta::core::Decimal{"6.5"});
}

TEST(DecimalTest, postfix_increment_decimal) {
  sourcemeta::core::Decimal value{"5.5"};
  value++;
  EXPECT_EQ(value, sourcemeta::core::Decimal{"6.5"});
}

TEST(DecimalTest, prefix_decrement_decimal) {
  sourcemeta::core::Decimal value{"5.5"};
  --value;
  EXPECT_EQ(value, sourcemeta::core::Decimal{"4.5"});
}

TEST(DecimalTest, postfix_decrement_decimal) {
  sourcemeta::core::Decimal value{"5.5"};
  value--;
  EXPECT_EQ(value, sourcemeta::core::Decimal{"4.5"});
}

TEST(DecimalTest, multiple_increments) {
  sourcemeta::core::Decimal value{0};
  ++value;
  ++value;
  ++value;
  EXPECT_EQ(value, sourcemeta::core::Decimal{3});
}

TEST(DecimalTest, multiple_decrements) {
  sourcemeta::core::Decimal value{10};
  value--;
  value--;
  value--;
  EXPECT_EQ(value, sourcemeta::core::Decimal{7});
}

TEST(DecimalTest, chained_prefix_operations) {
  sourcemeta::core::Decimal value{5};
  const sourcemeta::core::Decimal result{++(++value)};
  EXPECT_EQ(value, sourcemeta::core::Decimal{7});
  EXPECT_EQ(result, sourcemeta::core::Decimal{7});
}

TEST(DecimalTest, default_constructor) {
  const sourcemeta::core::Decimal value;
  EXPECT_TRUE(value.is_zero());
  EXPECT_EQ(value, sourcemeta::core::Decimal{0});
}

TEST(DecimalTest, copy_constructor) {
  const sourcemeta::core::Decimal original{42};
  const sourcemeta::core::Decimal copy{original};
  EXPECT_EQ(copy, original);
  EXPECT_EQ(copy, sourcemeta::core::Decimal{42});
}

TEST(DecimalTest, copy_constructor_decimal_value) {
  const sourcemeta::core::Decimal original{"3.14159"};
  const sourcemeta::core::Decimal copy{original};
  EXPECT_EQ(copy, original);
}

TEST(DecimalTest, copy_assignment) {
  const sourcemeta::core::Decimal original{100};
  sourcemeta::core::Decimal copy{0};
  copy = original;
  EXPECT_EQ(copy, original);
  EXPECT_EQ(copy, sourcemeta::core::Decimal{100});
}

TEST(DecimalTest, move_constructor) {
  sourcemeta::core::Decimal original{999};
  const sourcemeta::core::Decimal moved{std::move(original)};
  EXPECT_EQ(moved, sourcemeta::core::Decimal{999});
}

TEST(DecimalTest, move_constructor_decimal_value) {
  sourcemeta::core::Decimal original{"2.71828"};
  const sourcemeta::core::Decimal moved{std::move(original)};
  EXPECT_EQ(moved, sourcemeta::core::Decimal{"2.71828"});
}

TEST(DecimalTest, move_assignment) {
  sourcemeta::core::Decimal original{500};
  sourcemeta::core::Decimal moved{0};
  moved = std::move(original);
  EXPECT_EQ(moved, sourcemeta::core::Decimal{500});
}

TEST(DecimalTest, copy_then_modify) {
  const sourcemeta::core::Decimal original{10};
  sourcemeta::core::Decimal copy{original};
  copy += sourcemeta::core::Decimal{5};
  EXPECT_EQ(original, sourcemeta::core::Decimal{10});
  EXPECT_EQ(copy, sourcemeta::core::Decimal{15});
}

TEST(DecimalTest, multiple_copies) {
  const sourcemeta::core::Decimal value1{7};
  const sourcemeta::core::Decimal value2{value1};
  const sourcemeta::core::Decimal value3{value2};
  EXPECT_EQ(value1, value2);
  EXPECT_EQ(value2, value3);
  EXPECT_EQ(value1, value3);
}

TEST(DecimalTest, compound_assignment_add) {
  sourcemeta::core::Decimal value{10};
  value += sourcemeta::core::Decimal{5};
  EXPECT_EQ(value, sourcemeta::core::Decimal{15});
}

TEST(DecimalTest, compound_assignment_subtract) {
  sourcemeta::core::Decimal value{20};
  value -= sourcemeta::core::Decimal{8};
  EXPECT_EQ(value, sourcemeta::core::Decimal{12});
}

TEST(DecimalTest, compound_assignment_multiply) {
  sourcemeta::core::Decimal value{6};
  value *= sourcemeta::core::Decimal{7};
  EXPECT_EQ(value, sourcemeta::core::Decimal{42});
}

TEST(DecimalTest, compound_assignment_divide) {
  sourcemeta::core::Decimal value{100};
  value /= sourcemeta::core::Decimal{5};
  EXPECT_EQ(value, sourcemeta::core::Decimal{20});
}

TEST(DecimalTest, compound_assignment_modulo) {
  sourcemeta::core::Decimal value{17};
  value %= sourcemeta::core::Decimal{5};
  EXPECT_EQ(value, sourcemeta::core::Decimal{2});
}

TEST(DecimalTest, compound_assignment_chain) {
  sourcemeta::core::Decimal value{10};
  value += sourcemeta::core::Decimal{5};
  value *= sourcemeta::core::Decimal{2};
  value -= sourcemeta::core::Decimal{10};
  EXPECT_EQ(value, sourcemeta::core::Decimal{20});
}

TEST(DecimalTest, to_string_integer) {
  const sourcemeta::core::Decimal value{12345};
  EXPECT_EQ(value.to_string(), "12345");
}

TEST(DecimalTest, to_string_decimal) {
  const sourcemeta::core::Decimal value{"123.456"};
  EXPECT_EQ(value.to_string(), "123.456");
}

TEST(DecimalTest, to_string_large_number) {
  const sourcemeta::core::Decimal value{"123456789012345678901234567890"};
  EXPECT_EQ(value.to_string(), "123456789012345678901234567890");
}

TEST(DecimalTest, is_int32_true) {
  const sourcemeta::core::Decimal value{1000};
  EXPECT_TRUE(value.is_int32());
}

TEST(DecimalTest, is_int32_false_too_large) {
  const sourcemeta::core::Decimal value{"3000000000"};
  EXPECT_FALSE(value.is_int32());
}

TEST(DecimalTest, is_int32_min_max) {
  const sourcemeta::core::Decimal min{-2147483648LL};
  const sourcemeta::core::Decimal max{2147483647};
  EXPECT_TRUE(min.is_int32());
  EXPECT_TRUE(max.is_int32());
}

TEST(DecimalTest, is_int64_true) {
  const sourcemeta::core::Decimal value{123456789012345LL};
  EXPECT_TRUE(value.is_int64());
}

TEST(DecimalTest, is_int64_false_too_large) {
  const sourcemeta::core::Decimal value{"99999999999999999999"};
  EXPECT_FALSE(value.is_int64());
}

TEST(DecimalTest, is_uint32_true) {
  const sourcemeta::core::Decimal value{4000000000U};
  EXPECT_TRUE(value.is_uint32());
}

TEST(DecimalTest, is_uint32_false_too_large) {
  const sourcemeta::core::Decimal value{"5000000000"};
  EXPECT_FALSE(value.is_uint32());
}

TEST(DecimalTest, is_uint32_max) {
  const sourcemeta::core::Decimal max{4294967295U};
  EXPECT_TRUE(max.is_uint32());
}

TEST(DecimalTest, is_uint64_true) {
  const sourcemeta::core::Decimal value{"18446744073709551615"};
  EXPECT_TRUE(value.is_uint64());
}

TEST(DecimalTest, is_uint64_false_too_large) {
  const sourcemeta::core::Decimal value{"18446744073709551616"};
  EXPECT_FALSE(value.is_uint64());
}

TEST(DecimalTest, exception_conversion_syntax_invalid_string) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"not_a_number"}; },
      sourcemeta::core::DecimalParseError);
}

TEST(DecimalTest, exception_conversion_syntax_empty_string) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{""}; },
      sourcemeta::core::DecimalParseError);
}

TEST(DecimalTest, exception_conversion_syntax_invalid_exponent) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"123e"}; },
      sourcemeta::core::DecimalParseError);
}

TEST(DecimalTest, exception_conversion_syntax_multiple_dots) {
  EXPECT_THROW(
      { const sourcemeta::core::Decimal value{"12.34.56"}; },
      sourcemeta::core::DecimalParseError);
}

TEST(DecimalTest, exception_overflow_multiplication) {
  const sourcemeta::core::Decimal large{"9e999999999999999999"};
  const sourcemeta::core::Decimal multiplier{10};
  EXPECT_THROW(
      { const auto result = large * multiplier; },
      sourcemeta::core::NumericOverflowError);
}

TEST(DecimalTest, exception_overflow_addition) {
  const sourcemeta::core::Decimal large{"9e999999999999999999"};
  const sourcemeta::core::Decimal addend{"9e999999999999999999"};
  EXPECT_THROW(
      { const auto result = large + addend; },
      sourcemeta::core::NumericOverflowError);
}

TEST(DecimalTest, negative_integer_copy) {
  const sourcemeta::core::Decimal original{-12345};
  const sourcemeta::core::Decimal copy{original};
  EXPECT_EQ(copy.to_int64(), -12345);
  EXPECT_TRUE(copy.is_signed());
}

TEST(DecimalTest, negative_integer_move) {
  sourcemeta::core::Decimal original{-98765};
  const sourcemeta::core::Decimal moved{std::move(original)};
  EXPECT_EQ(moved.to_int64(), -98765);
  EXPECT_TRUE(moved.is_signed());
}

TEST(DecimalTest, positive_not_signed_after_copy) {
  const sourcemeta::core::Decimal original{"123.456"};
  const sourcemeta::core::Decimal copy{original};
  EXPECT_FALSE(copy.is_signed());
}

TEST(DecimalTest, positive_not_signed_after_move) {
  sourcemeta::core::Decimal original{"789.012"};
  const sourcemeta::core::Decimal moved{std::move(original)};
  EXPECT_FALSE(moved.is_signed());
}

TEST(DecimalTest, zero_not_signed_after_copy) {
  const sourcemeta::core::Decimal original{0};
  const sourcemeta::core::Decimal copy{original};
  EXPECT_FALSE(copy.is_signed());
}

TEST(DecimalTest, construct_from_float_simple) {
  const float value{3.5f};
  const sourcemeta::core::Decimal decimal{value};
  EXPECT_TRUE(decimal.is_float());
  EXPECT_TRUE(decimal.is_double());
  EXPECT_EQ(decimal.to_float(), 3.5f);
  EXPECT_EQ(decimal.to_double(), 3.5);
  EXPECT_EQ(decimal.to_string(), "3.5");
}

TEST(DecimalTest, construct_from_float_zero) {
  const float value{0.0f};
  const sourcemeta::core::Decimal decimal{value};
  EXPECT_TRUE(decimal.is_float());
  EXPECT_TRUE(decimal.is_double());
  EXPECT_TRUE(decimal.is_zero());
  EXPECT_EQ(decimal.to_float(), 0.0f);
  EXPECT_EQ(decimal.to_double(), 0.0);
}

TEST(DecimalTest, construct_from_float_very_small) {
  const float value{0.125f};
  const sourcemeta::core::Decimal decimal{value};
  EXPECT_TRUE(decimal.is_float());
  EXPECT_TRUE(decimal.is_double());
  EXPECT_EQ(decimal.to_float(), 0.125f);
  EXPECT_EQ(decimal.to_double(), 0.125);
  EXPECT_EQ(decimal.to_string(), "0.125");
}

TEST(DecimalTest, construct_from_float_roundtrip) {
  const float value{3.2f};
  const sourcemeta::core::Decimal decimal{value};
  EXPECT_TRUE(decimal.is_float());
  const float roundtrip{decimal.to_float()};
  EXPECT_EQ(roundtrip, value);
}

TEST(DecimalTest, multithreaded_construction) {
  constexpr int number_of_threads{4};
  std::vector<std::thread> threads;
  threads.reserve(number_of_threads);

  for (int index = 0; index < number_of_threads; index++) {
    threads.emplace_back([]() {
      const sourcemeta::core::Decimal value{42};
      const sourcemeta::core::Decimal result{value +
                                             sourcemeta::core::Decimal{1}};
      EXPECT_EQ(result, sourcemeta::core::Decimal{43});
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

TEST(DecimalTest, factory_snan) {
  const auto snan_value{sourcemeta::core::Decimal::snan()};
  EXPECT_TRUE(snan_value.is_nan());
  EXPECT_TRUE(snan_value.is_snan());
  EXPECT_FALSE(snan_value.is_qnan());
  EXPECT_FALSE(snan_value.is_finite());
  EXPECT_FALSE(snan_value.is_infinite());
}

TEST(DecimalTest, factory_nan_with_payload) {
  const auto nan_value{sourcemeta::core::Decimal::nan(123)};
  EXPECT_TRUE(nan_value.is_nan());
  EXPECT_TRUE(nan_value.is_qnan());
  EXPECT_FALSE(nan_value.is_snan());
  EXPECT_EQ(nan_value.nan_payload(), 123);
}

TEST(DecimalTest, factory_snan_with_payload) {
  const auto snan_value{sourcemeta::core::Decimal::snan(456)};
  EXPECT_TRUE(snan_value.is_nan());
  EXPECT_TRUE(snan_value.is_snan());
  EXPECT_FALSE(snan_value.is_qnan());
  EXPECT_EQ(snan_value.nan_payload(), 456);
}

TEST(DecimalTest, is_snan_false_for_qnan) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_FALSE(nan_value.is_snan());
  EXPECT_TRUE(nan_value.is_qnan());
}

TEST(DecimalTest, is_qnan_false_for_snan) {
  const auto snan_value{sourcemeta::core::Decimal::snan()};
  EXPECT_FALSE(snan_value.is_qnan());
  EXPECT_TRUE(snan_value.is_snan());
}

TEST(DecimalTest, is_nan_true_for_both) {
  EXPECT_TRUE(sourcemeta::core::Decimal::nan().is_nan());
  EXPECT_TRUE(sourcemeta::core::Decimal::snan().is_nan());
}

TEST(DecimalTest, parse_snan_string) {
  const sourcemeta::core::Decimal value{"sNaN"};
  EXPECT_TRUE(value.is_nan());
  EXPECT_TRUE(value.is_snan());
  EXPECT_EQ(value.nan_payload(), 0);
}

TEST(DecimalTest, parse_nan_with_payload_string) {
  const sourcemeta::core::Decimal value{"NaN123"};
  EXPECT_TRUE(value.is_nan());
  EXPECT_TRUE(value.is_qnan());
  EXPECT_EQ(value.nan_payload(), 123);
}

TEST(DecimalTest, parse_snan_with_payload_string) {
  const sourcemeta::core::Decimal value{"sNaN789"};
  EXPECT_TRUE(value.is_nan());
  EXPECT_TRUE(value.is_snan());
  EXPECT_EQ(value.nan_payload(), 789);
}

TEST(DecimalTest, snan_arithmetic_produces_qnan) {
  const auto snan_value{sourcemeta::core::Decimal::snan()};
  const auto result{snan_value + sourcemeta::core::Decimal{1}};
  EXPECT_TRUE(result.is_nan());
  EXPECT_TRUE(result.is_qnan());
  EXPECT_FALSE(result.is_snan());
}

TEST(DecimalTest, equal_different_scale_integer) {
  EXPECT_EQ(sourcemeta::core::Decimal{"1.0"}, sourcemeta::core::Decimal{1});
  EXPECT_EQ(sourcemeta::core::Decimal{"2.00"}, sourcemeta::core::Decimal{2});
}

TEST(DecimalTest, equal_different_scale_decimal) {
  EXPECT_EQ(sourcemeta::core::Decimal{"0.10"},
            sourcemeta::core::Decimal{"0.1"});
}

TEST(DecimalTest, equal_different_scale_zero) {
  EXPECT_EQ(sourcemeta::core::Decimal{"0.0"}, sourcemeta::core::Decimal{0});
  EXPECT_EQ(sourcemeta::core::Decimal{"0.00"}, sourcemeta::core::Decimal{0});
}

TEST(DecimalTest, compare_nan_not_equal_to_self) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_FALSE(nan_value == nan_value);
}

TEST(DecimalTest, compare_nan_not_equal_to_nan) {
  EXPECT_FALSE(sourcemeta::core::Decimal::nan() ==
               sourcemeta::core::Decimal::nan());
}

TEST(DecimalTest, compare_nan_not_less_than_anything) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_FALSE(nan_value < sourcemeta::core::Decimal{5});
  EXPECT_FALSE(nan_value < sourcemeta::core::Decimal::infinity());
}

TEST(DecimalTest, compare_nan_not_greater_than_anything) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_FALSE(nan_value > sourcemeta::core::Decimal{5});
  EXPECT_FALSE(nan_value > sourcemeta::core::Decimal::negative_infinity());
}

TEST(DecimalTest, compare_nan_not_less_equal_anything) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_FALSE(nan_value <= sourcemeta::core::Decimal{5});
}

TEST(DecimalTest, compare_nan_not_greater_equal_anything) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_FALSE(nan_value >= sourcemeta::core::Decimal{5});
}

TEST(DecimalTest, compare_nan_not_equal_finite) {
  const auto nan_value{sourcemeta::core::Decimal::nan()};
  EXPECT_TRUE(nan_value != sourcemeta::core::Decimal{0});
  EXPECT_TRUE(nan_value != sourcemeta::core::Decimal{42});
}

TEST(DecimalTest, compare_very_different_magnitudes) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"1e-300"} <
              sourcemeta::core::Decimal{"1e300"});
}

TEST(DecimalTest, compare_adjacent_values) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"1.000000000000000"} <
              sourcemeta::core::Decimal{"1.000000000000001"});
}

TEST(DecimalTest, compare_sign_difference) {
  EXPECT_TRUE(sourcemeta::core::Decimal{5} > sourcemeta::core::Decimal{-5});
  EXPECT_TRUE(sourcemeta::core::Decimal{-5} < sourcemeta::core::Decimal{5});
}

TEST(DecimalTest, compare_reflexive_zero) {
  const sourcemeta::core::Decimal value{0};
  EXPECT_EQ(value, value);
}

TEST(DecimalTest, compare_reflexive_decimal) {
  const sourcemeta::core::Decimal value{"3.14"};
  EXPECT_EQ(value, value);
}

TEST(DecimalTest, compare_symmetric) {
  const sourcemeta::core::Decimal small{3};
  const sourcemeta::core::Decimal large{7};
  EXPECT_TRUE(small < large);
  EXPECT_TRUE(large > small);
}

TEST(DecimalTest, compare_transitive) {
  const sourcemeta::core::Decimal first{1};
  const sourcemeta::core::Decimal second{5};
  const sourcemeta::core::Decimal third{10};
  EXPECT_TRUE(first < second);
  EXPECT_TRUE(second < third);
  EXPECT_TRUE(first < third);
}

TEST(DecimalTest, divisible_by_one_tenth) {
  const sourcemeta::core::Decimal dividend{1};
  const sourcemeta::core::Decimal divisor{"0.1"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_two_tenths) {
  const sourcemeta::core::Decimal dividend{2};
  const sourcemeta::core::Decimal divisor{"0.1"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_three_tenths_true) {
  const sourcemeta::core::Decimal dividend{"0.3"};
  const sourcemeta::core::Decimal divisor{"0.1"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_six_tenths_by_two_tenths_true) {
  const sourcemeta::core::Decimal dividend{"0.6"};
  const sourcemeta::core::Decimal divisor{"0.2"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_twelve_tenths_by_four_tenths_true) {
  const sourcemeta::core::Decimal dividend{"1.2"};
  const sourcemeta::core::Decimal divisor{"0.4"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_ten_by_tenth_true) {
  const sourcemeta::core::Decimal dividend{10};
  const sourcemeta::core::Decimal divisor{"0.1"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_ten_by_third_false) {
  const sourcemeta::core::Decimal dividend{10};
  const sourcemeta::core::Decimal divisor{"0.3"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_one_by_thousandth_true) {
  const sourcemeta::core::Decimal dividend{1};
  const sourcemeta::core::Decimal divisor{"0.001"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_hundredths_true) {
  const sourcemeta::core::Decimal dividend{"0.0075"};
  const sourcemeta::core::Decimal divisor{"0.0001"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_hundredths_false) {
  const sourcemeta::core::Decimal dividend{"0.00751"};
  const sourcemeta::core::Decimal divisor{"0.0001"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_hundred_by_hundredth_true) {
  const sourcemeta::core::Decimal dividend{100};
  const sourcemeta::core::Decimal divisor{"0.01"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_tiny_self_divisible) {
  const sourcemeta::core::Decimal dividend{"0.0000001"};
  const sourcemeta::core::Decimal divisor{"0.0000001"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_large_integer_by_three_true) {
  const sourcemeta::core::Decimal dividend{"999999999999999999"};
  const sourcemeta::core::Decimal divisor{3};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_large_integer_by_three_false) {
  const sourcemeta::core::Decimal dividend{"999999999999999997"};
  const sourcemeta::core::Decimal divisor{3};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_large_quotient_exceeding_int64) {
  const sourcemeta::core::Decimal dividend{"99999999999999999999999999"};
  const sourcemeta::core::Decimal divisor{1};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_different_scale_alignment) {
  const sourcemeta::core::Decimal dividend{"1000000.000000"};
  const sourcemeta::core::Decimal divisor{"0.000001"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_seven_point_five_by_two_point_five_true) {
  const sourcemeta::core::Decimal dividend{"7.5"};
  const sourcemeta::core::Decimal divisor{"2.5"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_mixed_integer_decimal_true) {
  const sourcemeta::core::Decimal dividend{10};
  const sourcemeta::core::Decimal divisor{"2.5"};
  EXPECT_TRUE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_mixed_integer_decimal_false) {
  const sourcemeta::core::Decimal dividend{10};
  const sourcemeta::core::Decimal divisor{"3.3"};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, divisible_by_nan_dividend) {
  const auto dividend{sourcemeta::core::Decimal::nan()};
  const sourcemeta::core::Decimal divisor{5};
  EXPECT_FALSE(dividend.divisible_by(divisor));
}

TEST(DecimalTest, parse_leading_zeros) {
  const sourcemeta::core::Decimal value{"007"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{7});
}

TEST(DecimalTest, parse_leading_zeros_decimal) {
  const sourcemeta::core::Decimal value{"00.5"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{"0.5"});
}

TEST(DecimalTest, parse_explicit_plus_sign) {
  const sourcemeta::core::Decimal value{"+123"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{123});
}

TEST(DecimalTest, parse_explicit_plus_zero) {
  const sourcemeta::core::Decimal value{"+0"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{0});
}

TEST(DecimalTest, parse_explicit_plus_decimal) {
  const sourcemeta::core::Decimal value{"+1.5"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{"1.5"});
}

TEST(DecimalTest, parse_bare_decimal_point_leading) {
  const sourcemeta::core::Decimal value{".5"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{"0.5"});
}

TEST(DecimalTest, parse_bare_decimal_point_trailing) {
  const sourcemeta::core::Decimal value{"5."};
  EXPECT_EQ(value, sourcemeta::core::Decimal{5});
}

TEST(DecimalTest, parse_plus_zero) {
  const sourcemeta::core::Decimal value{"+0"};
  EXPECT_EQ(value, sourcemeta::core::Decimal{0});
  EXPECT_FALSE(value.is_signed());
}

TEST(DecimalTest, parse_special_nan) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"NaN"}.is_nan());
  EXPECT_TRUE(sourcemeta::core::Decimal{"nan"}.is_nan());
  EXPECT_TRUE(sourcemeta::core::Decimal{"NAN"}.is_nan());
}

TEST(DecimalTest, parse_special_inf_shorthand) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"Inf"}.is_infinite());
}

TEST(DecimalTest, parse_many_digits_integer) {
  const sourcemeta::core::Decimal value{
      "12345678901234567890123456789012345678901234567890"};
  EXPECT_FALSE(value.is_zero());
  EXPECT_TRUE(value.is_finite());
}

TEST(DecimalTest, parse_very_large_exponent) {
  const sourcemeta::core::Decimal value{"1e999999"};
  EXPECT_FALSE(value.is_zero());
  EXPECT_TRUE(value.is_finite());
}

TEST(DecimalTest, to_string_zero) {
  const sourcemeta::core::Decimal value{0};
  EXPECT_EQ(value.to_string(), "0");
}

TEST(DecimalTest, to_string_very_small_decimal) {
  const sourcemeta::core::Decimal value{"0.000000001"};
  EXPECT_EQ(value.to_string(), "1e-9");
}

TEST(DecimalTest, to_string_nan) {
  EXPECT_EQ(sourcemeta::core::Decimal::nan().to_string(), "NaN");
}

TEST(DecimalTest, add_nan_propagates) {
  const auto result{sourcemeta::core::Decimal::nan() +
                    sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(result.is_nan());
}

TEST(DecimalTest, multiply_nan_propagates) {
  const auto result{sourcemeta::core::Decimal::nan() *
                    sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(result.is_nan());
}

TEST(DecimalTest, divide_nan_propagates) {
  const auto result{sourcemeta::core::Decimal::nan() /
                    sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(result.is_nan());
}

TEST(DecimalTest, subtract_nan_propagates) {
  const auto result{sourcemeta::core::Decimal::nan() -
                    sourcemeta::core::Decimal{5}};
  EXPECT_TRUE(result.is_nan());
}

TEST(DecimalTest, add_different_scales) {
  EXPECT_EQ(sourcemeta::core::Decimal{"1.0"} + sourcemeta::core::Decimal{2},
            sourcemeta::core::Decimal{3});
  EXPECT_EQ(sourcemeta::core::Decimal{"10.00"} -
                sourcemeta::core::Decimal{"9.9"},
            sourcemeta::core::Decimal{"0.10"});
}

TEST(DecimalTest, subtract_different_scales) {
  EXPECT_EQ(sourcemeta::core::Decimal{"10.000"} -
                sourcemeta::core::Decimal{"0.1"},
            sourcemeta::core::Decimal{"9.900"});
}

TEST(DecimalTest, add_classic_fp_trouble_exact_in_decimal) {
  const auto result{sourcemeta::core::Decimal{"0.1"} +
                    sourcemeta::core::Decimal{"0.2"}};
  EXPECT_EQ(result, sourcemeta::core::Decimal{"0.3"});
}

TEST(DecimalTest, commutativity_addition) {
  const sourcemeta::core::Decimal first{"3.14"};
  const sourcemeta::core::Decimal second{"2.71"};
  EXPECT_EQ(first + second, second + first);
}

TEST(DecimalTest, commutativity_multiplication) {
  const sourcemeta::core::Decimal first{"3.14"};
  const sourcemeta::core::Decimal second{"2.71"};
  EXPECT_EQ(first * second, second * first);
}

TEST(DecimalTest, additive_identity_decimal) {
  EXPECT_EQ(sourcemeta::core::Decimal{"3.14"} + sourcemeta::core::Decimal{0},
            sourcemeta::core::Decimal{"3.14"});
}

TEST(DecimalTest, additive_identity_large) {
  EXPECT_EQ(sourcemeta::core::Decimal{"1e100"} + sourcemeta::core::Decimal{0},
            sourcemeta::core::Decimal{"1e100"});
}

TEST(DecimalTest, multiplicative_identity_decimal) {
  EXPECT_EQ(sourcemeta::core::Decimal{"3.14"} * sourcemeta::core::Decimal{1},
            sourcemeta::core::Decimal{"3.14"});
}

TEST(DecimalTest, multiplicative_identity_large) {
  EXPECT_EQ(sourcemeta::core::Decimal{"1e100"} * sourcemeta::core::Decimal{1},
            sourcemeta::core::Decimal{"1e100"});
}

TEST(DecimalTest, divide_self_equals_one_decimal) {
  const sourcemeta::core::Decimal value{"3.14"};
  EXPECT_EQ(value / value, sourcemeta::core::Decimal{1});
}

TEST(DecimalTest, divide_self_equals_one_small) {
  const sourcemeta::core::Decimal value{"0.001"};
  EXPECT_EQ(value / value, sourcemeta::core::Decimal{1});
}

TEST(DecimalTest, modulo_decimal_operands) {
  EXPECT_EQ(sourcemeta::core::Decimal{"10.5"} % sourcemeta::core::Decimal{3},
            sourcemeta::core::Decimal{"1.5"});
  EXPECT_EQ(sourcemeta::core::Decimal{"7.5"} % sourcemeta::core::Decimal{"2.5"},
            sourcemeta::core::Decimal{"0.0"});
}

TEST(DecimalTest, modulo_result_zero) {
  EXPECT_EQ(sourcemeta::core::Decimal{100} % sourcemeta::core::Decimal{25},
            sourcemeta::core::Decimal{0});
}

TEST(DecimalTest, modulo_dividend_less_than_divisor) {
  EXPECT_EQ(sourcemeta::core::Decimal{3} % sourcemeta::core::Decimal{7},
            sourcemeta::core::Decimal{3});
}

TEST(DecimalTest, modulo_dividend_equals_divisor) {
  EXPECT_EQ(sourcemeta::core::Decimal{7} % sourcemeta::core::Decimal{7},
            sourcemeta::core::Decimal{0});
}

TEST(DecimalTest, is_uint32_at_zero) {
  const sourcemeta::core::Decimal value{0};
  EXPECT_TRUE(value.is_uint32());
}

TEST(DecimalTest, is_uint64_at_zero) {
  const sourcemeta::core::Decimal value{0};
  EXPECT_TRUE(value.is_uint64());
}

TEST(DecimalTest, to_int32_at_boundaries) {
  const sourcemeta::core::Decimal min_value{
      static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::min())};
  const sourcemeta::core::Decimal max_value{
      std::numeric_limits<std::int32_t>::max()};
  EXPECT_EQ(min_value.to_int32(), std::numeric_limits<std::int32_t>::min());
  EXPECT_EQ(max_value.to_int32(), std::numeric_limits<std::int32_t>::max());
}

TEST(DecimalTest, to_int64_at_boundaries) {
  const sourcemeta::core::Decimal min_value{"-9223372036854775808"};
  const sourcemeta::core::Decimal max_value{"9223372036854775807"};
  EXPECT_EQ(min_value.to_int64(), std::numeric_limits<std::int64_t>::min());
  EXPECT_EQ(max_value.to_int64(), std::numeric_limits<std::int64_t>::max());
}

TEST(DecimalTest, to_uint32_at_boundaries) {
  const sourcemeta::core::Decimal zero{0};
  const sourcemeta::core::Decimal max_value{
      std::numeric_limits<std::uint32_t>::max()};
  EXPECT_EQ(zero.to_uint32(), 0U);
  EXPECT_EQ(max_value.to_uint32(), std::numeric_limits<std::uint32_t>::max());
}

TEST(DecimalTest, to_uint64_at_boundaries) {
  const sourcemeta::core::Decimal zero{0};
  const sourcemeta::core::Decimal max_value{"18446744073709551615"};
  EXPECT_EQ(zero.to_uint64(), 0ULL);
  EXPECT_EQ(max_value.to_uint64(), std::numeric_limits<std::uint64_t>::max());
}

TEST(DecimalTest, int_conversion_power_of_ten_boundaries) {
  const sourcemeta::core::Decimal value{"10000000000000000000"};
  EXPECT_TRUE(value.is_integer());
  EXPECT_FALSE(value.is_int64());
  EXPECT_TRUE(value.is_uint64());
}

TEST(DecimalTest, construct_from_int64_min) {
  const sourcemeta::core::Decimal value{
      std::numeric_limits<std::int64_t>::min()};
  EXPECT_TRUE(value.is_signed());
  EXPECT_TRUE(value.is_integer());
  EXPECT_TRUE(value.is_int64());
  EXPECT_EQ(value.to_int64(), std::numeric_limits<std::int64_t>::min());
}

TEST(DecimalTest, construct_from_int64_max) {
  const sourcemeta::core::Decimal value{
      std::numeric_limits<std::int64_t>::max()};
  EXPECT_FALSE(value.is_signed());
  EXPECT_TRUE(value.is_integer());
  EXPECT_TRUE(value.is_int64());
  EXPECT_EQ(value.to_int64(), std::numeric_limits<std::int64_t>::max());
}

TEST(DecimalTest, construct_from_uint64_max) {
  const sourcemeta::core::Decimal value{
      std::numeric_limits<std::uint64_t>::max()};
  EXPECT_FALSE(value.is_signed());
  EXPECT_TRUE(value.is_integer());
  EXPECT_TRUE(value.is_uint64());
  EXPECT_EQ(value.to_uint64(), std::numeric_limits<std::uint64_t>::max());
}

TEST(DecimalTest, construct_from_int8) {
  const sourcemeta::core::Decimal value{static_cast<std::int8_t>(-42)};
  EXPECT_EQ(value, sourcemeta::core::Decimal{-42});
}

TEST(DecimalTest, construct_from_int16) {
  const sourcemeta::core::Decimal value{static_cast<std::int16_t>(-1000)};
  EXPECT_EQ(value, sourcemeta::core::Decimal{-1000});
}

TEST(DecimalTest, construct_from_uint8) {
  const sourcemeta::core::Decimal value{static_cast<std::uint8_t>(255)};
  EXPECT_EQ(value, sourcemeta::core::Decimal{255});
}

TEST(DecimalTest, construct_from_uint16) {
  const sourcemeta::core::Decimal value{static_cast<std::uint16_t>(65535)};
  EXPECT_EQ(value, sourcemeta::core::Decimal{65535});
}

TEST(DecimalTest, construct_from_int32) {
  const sourcemeta::core::Decimal value{static_cast<std::int32_t>(-2000000)};
  EXPECT_EQ(value, sourcemeta::core::Decimal{-2000000});
}

TEST(DecimalTest, construct_from_uint32_explicit) {
  const sourcemeta::core::Decimal value{static_cast<std::uint32_t>(4000000000)};
  EXPECT_EQ(value.to_string(), "4000000000");
}

TEST(DecimalTest, construct_from_float_nan) {
  const sourcemeta::core::Decimal value{std::nanf("")};
  EXPECT_TRUE(value.is_nan());
}

TEST(DecimalTest, self_copy_assignment) {
  sourcemeta::core::Decimal value{42};
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif
  value = value;
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
  EXPECT_EQ(value, sourcemeta::core::Decimal{42});
}

TEST(DecimalTest, self_move_assignment) {
  sourcemeta::core::Decimal value{42};
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
#endif
  value = std::move(value);
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
  EXPECT_EQ(value, sourcemeta::core::Decimal{42});
}

TEST(DecimalTest, move_from_state_is_valid) {
  sourcemeta::core::Decimal original{42};
  const sourcemeta::core::Decimal moved{std::move(original)};
  EXPECT_EQ(moved, sourcemeta::core::Decimal{42});
  original = sourcemeta::core::Decimal{99};
  EXPECT_EQ(original, sourcemeta::core::Decimal{99});
}

TEST(DecimalTest, unary_minus_zero) {
  const sourcemeta::core::Decimal value{0};
  const auto result{-value};
  EXPECT_TRUE(result.is_zero());
}

TEST(DecimalTest, unary_minus_nan) {
  const auto result{-sourcemeta::core::Decimal::nan()};
  EXPECT_TRUE(result.is_nan());
}

TEST(DecimalTest, unary_plus_nan) {
  const auto result{+sourcemeta::core::Decimal::nan()};
  EXPECT_TRUE(result.is_nan());
}

TEST(DecimalTest, double_negation_identity_decimal) {
  const sourcemeta::core::Decimal value{"3.14"};
  EXPECT_EQ(-(-value), value);
}

TEST(DecimalTest, double_negation_identity_zero) {
  const sourcemeta::core::Decimal value{0};
  EXPECT_EQ(-(-value), value);
}

TEST(DecimalTest, is_finite_nan) {
  EXPECT_FALSE(sourcemeta::core::Decimal::nan().is_finite());
}

TEST(DecimalTest, compound_add_self_doubles) {
  sourcemeta::core::Decimal value{25};
  value += value;
  EXPECT_EQ(value, sourcemeta::core::Decimal{50});
}

TEST(DecimalTest, compound_subtract_self_zeros) {
  sourcemeta::core::Decimal value{25};
  value -= value;
  EXPECT_TRUE(value.is_zero());
}

TEST(DecimalTest, compound_multiply_self_squares) {
  sourcemeta::core::Decimal value{5};
  value *= value;
  EXPECT_EQ(value, sourcemeta::core::Decimal{25});
}

TEST(DecimalTest, compound_divide_self_gives_one) {
  sourcemeta::core::Decimal value{42};
  value /= value;
  EXPECT_EQ(value, sourcemeta::core::Decimal{1});
}

TEST(DecimalTest, compound_modulo_self_gives_zero) {
  sourcemeta::core::Decimal value{42};
  value %= value;
  EXPECT_TRUE(value.is_zero());
}

TEST(DecimalTest, multithreaded_parsing) {
  constexpr int number_of_threads{16};
  std::vector<std::thread> threads;
  threads.reserve(number_of_threads);

  for (int index = 0; index < number_of_threads; index++) {
    threads.emplace_back([]() {
      const sourcemeta::core::Decimal value{"3.14159265358979"};
      EXPECT_FALSE(value.is_zero());
      EXPECT_TRUE(value.is_finite());
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

TEST(DecimalTest, multithreaded_stringification) {
  constexpr int number_of_threads{16};
  std::vector<std::thread> threads;
  threads.reserve(number_of_threads);

  for (int index = 0; index < number_of_threads; index++) {
    threads.emplace_back([]() {
      const sourcemeta::core::Decimal value{12345};
      EXPECT_EQ(value.to_string(), "12345");
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

TEST(DecimalTest, multithreaded_comparison) {
  constexpr int number_of_threads{16};
  std::vector<std::thread> threads;
  threads.reserve(number_of_threads);

  for (int index = 0; index < number_of_threads; index++) {
    threads.emplace_back([]() {
      const sourcemeta::core::Decimal left{100};
      const sourcemeta::core::Decimal right{200};
      EXPECT_TRUE(left < right);
      EXPECT_TRUE(right > left);
      EXPECT_EQ(left, left);
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

TEST(DecimalTest, multithreaded_divisibility) {
  constexpr int number_of_threads{16};
  std::vector<std::thread> threads;
  threads.reserve(number_of_threads);

  for (int index = 0; index < number_of_threads; index++) {
    threads.emplace_back([]() {
      const sourcemeta::core::Decimal dividend{100};
      const sourcemeta::core::Decimal divisor{5};
      EXPECT_TRUE(dividend.divisible_by(divisor));
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

TEST(DecimalTest, multithreaded_mixed_operations) {
  constexpr int number_of_threads{16};
  std::vector<std::thread> threads;
  threads.reserve(number_of_threads);

  for (int index = 0; index < number_of_threads; index++) {
    threads.emplace_back([index]() {
      if (index % 4 == 0) {
        EXPECT_EQ(sourcemeta::core::Decimal{10} + sourcemeta::core::Decimal{5},
                  sourcemeta::core::Decimal{15});
      } else if (index % 4 == 1) {
        EXPECT_EQ(sourcemeta::core::Decimal{10} * sourcemeta::core::Decimal{3},
                  sourcemeta::core::Decimal{30});
      } else if (index % 4 == 2) {
        EXPECT_EQ(sourcemeta::core::Decimal{10} / sourcemeta::core::Decimal{2},
                  sourcemeta::core::Decimal{5});
      } else {
        EXPECT_EQ(sourcemeta::core::Decimal{"3.14"}.to_string(), "3.14");
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

TEST(DecimalTest, multithreaded_high_thread_count) {
  constexpr int number_of_threads{32};
  std::vector<std::thread> threads;
  threads.reserve(number_of_threads);

  for (int index = 0; index < number_of_threads; index++) {
    threads.emplace_back([]() {
      const sourcemeta::core::Decimal value{42};
      const sourcemeta::core::Decimal result{value +
                                             sourcemeta::core::Decimal{1}};
      EXPECT_EQ(result, sourcemeta::core::Decimal{43});
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
}

TEST(DecimalTest, reduce_strips_trailing_zeros) {
  const sourcemeta::core::Decimal value{"1.200"};
  const sourcemeta::core::Decimal expected{"1.2"};
  EXPECT_EQ(value.reduce(), expected);
}

TEST(DecimalTest, reduce_zero) {
  const sourcemeta::core::Decimal value{"0.00"};
  const sourcemeta::core::Decimal expected{0};
  EXPECT_EQ(value.reduce(), expected);
}

TEST(DecimalTest, reduce_integer) {
  const sourcemeta::core::Decimal value{"1200"};
  const sourcemeta::core::Decimal expected{"12E+2"};
  EXPECT_EQ(value.reduce(), expected);
}

TEST(DecimalTest, reduce_nan) {
  EXPECT_TRUE(sourcemeta::core::Decimal::nan().reduce().is_nan());
}

TEST(DecimalTest, logb_positive) {
  const sourcemeta::core::Decimal value{"250"};
  EXPECT_EQ(value.logb(), sourcemeta::core::Decimal{2});
}

TEST(DecimalTest, logb_one) {
  const sourcemeta::core::Decimal value{1};
  EXPECT_EQ(value.logb(), sourcemeta::core::Decimal{0});
}

TEST(DecimalTest, logb_fraction) {
  const sourcemeta::core::Decimal value{"0.001"};
  EXPECT_EQ(value.logb(), sourcemeta::core::Decimal{-3});
}

TEST(DecimalTest, logb_nan) {
  EXPECT_TRUE(sourcemeta::core::Decimal::nan().logb().is_nan());
}

TEST(DecimalTest, scale_by_positive) {
  const sourcemeta::core::Decimal value{"1.23"};
  const sourcemeta::core::Decimal scale{2};
  const sourcemeta::core::Decimal expected{"123"};
  EXPECT_EQ(value.scale_by(scale), expected);
}

TEST(DecimalTest, same_quantum_same_exponent) {
  const sourcemeta::core::Decimal left{"1.23"};
  const sourcemeta::core::Decimal right{"4.56"};
  EXPECT_TRUE(left.same_quantum(right));
}

TEST(DecimalTest, same_quantum_different_exponent) {
  const sourcemeta::core::Decimal left{"1.2"};
  const sourcemeta::core::Decimal right{"1.23"};
  EXPECT_FALSE(left.same_quantum(right));
}

TEST(DecimalTest, same_quantum_both_nan) {
  EXPECT_TRUE(sourcemeta::core::Decimal::nan().same_quantum(
      sourcemeta::core::Decimal::nan()));
}

TEST(DecimalTest, same_quantum_nan_and_number) {
  EXPECT_FALSE(sourcemeta::core::Decimal::nan().same_quantum(
      sourcemeta::core::Decimal{1}));
}

TEST(DecimalTest, compare_total_positive_ordering) {
  const sourcemeta::core::Decimal left{7};
  const sourcemeta::core::Decimal right{10};
  EXPECT_EQ(left.compare_total(right), sourcemeta::core::Decimal{-1});
}

TEST(DecimalTest, compare_total_equal) {
  const sourcemeta::core::Decimal value{5};
  EXPECT_EQ(value.compare_total(value), sourcemeta::core::Decimal{0});
}

TEST(DecimalTest, compare_total_same_value_different_exponent) {
  const sourcemeta::core::Decimal left{"7.0"};
  const sourcemeta::core::Decimal right{7};
  EXPECT_EQ(left.compare_total(right), sourcemeta::core::Decimal{-1});
}

TEST(DecimalTest, compare_total_nan_ordering) {
  EXPECT_EQ(sourcemeta::core::Decimal::nan().compare_total(
                sourcemeta::core::Decimal{1}),
            sourcemeta::core::Decimal{1});
}

TEST(DecimalTest, divide_integer_exact) {
  const sourcemeta::core::Decimal left{7};
  const sourcemeta::core::Decimal right{2};
  EXPECT_EQ(left.divide_integer(right), sourcemeta::core::Decimal{3});
}

TEST(DecimalTest, divide_integer_by_larger) {
  const sourcemeta::core::Decimal left{2};
  const sourcemeta::core::Decimal right{7};
  EXPECT_EQ(left.divide_integer(right), sourcemeta::core::Decimal{0});
}

TEST(DecimalTest, divide_integer_with_decimals) {
  const sourcemeta::core::Decimal left{"10.5"};
  const sourcemeta::core::Decimal right{"3.1"};
  EXPECT_EQ(left.divide_integer(right), sourcemeta::core::Decimal{3});
}

TEST(DecimalTest, strict_from_positive_integer) {
  const auto result{sourcemeta::core::Decimal::strict_from(5.0)};
  EXPECT_EQ(result, sourcemeta::core::Decimal{5});
}

TEST(DecimalTest, strict_from_zero) {
  const auto result{sourcemeta::core::Decimal::strict_from(0.0)};
  EXPECT_EQ(result, sourcemeta::core::Decimal{0});
}

TEST(DecimalTest, strict_from_0_1) {
  const auto result{sourcemeta::core::Decimal::strict_from(0.1)};
  EXPECT_EQ(result, sourcemeta::core::Decimal{"0.1"});
}

TEST(DecimalTest, strict_from_0_01) {
  const auto result{sourcemeta::core::Decimal::strict_from(0.01)};
  EXPECT_EQ(result, sourcemeta::core::Decimal{"0.01"});
}

TEST(DecimalTest, strict_from_0_001) {
  const auto result{sourcemeta::core::Decimal::strict_from(0.001)};
  EXPECT_EQ(result, sourcemeta::core::Decimal{"0.001"});
}

TEST(DecimalTest, strict_from_0_0001) {
  const auto result{sourcemeta::core::Decimal::strict_from(0.0001)};
  EXPECT_EQ(result, sourcemeta::core::Decimal{"0.0001"});
}

TEST(DecimalTest, strict_from_1280_32) {
  const auto result{sourcemeta::core::Decimal::strict_from(1280.32)};
  EXPECT_EQ(result, sourcemeta::core::Decimal{"1280.32"});
}

TEST(DecimalTest, strict_from_99_99) {
  const auto result{sourcemeta::core::Decimal::strict_from(99.99)};
  EXPECT_EQ(result, sourcemeta::core::Decimal{"99.99"});
}

TEST(DecimalTest, strict_from_large_value) {
  const auto result{sourcemeta::core::Decimal::strict_from(999999.99)};
  EXPECT_EQ(result, sourcemeta::core::Decimal{"999999.99"});
}

TEST(DecimalTest, parse_long_integer_literal_preserves_ordering) {
  const std::string shorter(2000, '1');
  std::string longer(2001, '1');
  EXPECT_TRUE(sourcemeta::core::Decimal{shorter} <
              sourcemeta::core::Decimal{longer});
}

TEST(DecimalTest,
     divide_integer_large_valid_finite_input_not_fabricated_sentinel) {
  const auto value{
      sourcemeta::core::Decimal{"1234567890123456789012345678901234567890"
                                "1234567890123456789012345678901234567890"
                                "12345678901234567890"}};
  const auto quotient{value.divide_integer(sourcemeta::core::Decimal{1})};
  EXPECT_EQ(quotient, value);
  EXPECT_FALSE(quotient.is_nan());
}

TEST(DecimalTest, divide_integer_large_same_limb_completes_promptly) {
  const auto dividend =
      sourcemeta::core::Decimal{"999999999999999999999999999999999999"};
  const auto divisor = sourcemeta::core::Decimal{"1000000000000000000"};
  const auto expected = sourcemeta::core::Decimal{"999999999999999999"};
  EXPECT_EQ(dividend.divide_integer(divisor), expected);
}

TEST(DecimalTest, divide_integer_large_same_limb_nearby_divisor) {
  const auto dividend =
      sourcemeta::core::Decimal{"999999999999999999999999999999999999"};
  const auto divisor = sourcemeta::core::Decimal{"1000000000000000001"};
  EXPECT_TRUE(dividend.divide_integer(divisor).is_finite());
}

TEST(DecimalTest, add_nan_left_propagates_payload_and_sign) {
  const auto left = -sourcemeta::core::Decimal::nan(123);
  const auto right = sourcemeta::core::Decimal{5};
  const auto result = left + right;
  EXPECT_TRUE(result.is_qnan());
  EXPECT_EQ(result.nan_payload(), 123u);
  EXPECT_TRUE(result.is_signed());
}

TEST(DecimalTest, add_nan_right_propagates_payload_and_sign) {
  const auto left = sourcemeta::core::Decimal{5};
  const auto right = sourcemeta::core::Decimal::nan(456);
  const auto result = left + right;
  EXPECT_TRUE(result.is_qnan());
  EXPECT_EQ(result.nan_payload(), 456u);
  EXPECT_FALSE(result.is_signed());
}

TEST(DecimalTest, add_both_nan_selects_left_payload) {
  const auto left = sourcemeta::core::Decimal::nan(5);
  const auto right = sourcemeta::core::Decimal::nan(6);
  const auto result = left + right;
  EXPECT_TRUE(result.is_qnan());
  EXPECT_EQ(result.nan_payload(), 5u);
}

TEST(DecimalTest, subtract_nan_propagates_payload) {
  const auto left = sourcemeta::core::Decimal::nan(7);
  const auto right = sourcemeta::core::Decimal{1};
  const auto result = left - right;
  EXPECT_TRUE(result.is_qnan());
  EXPECT_EQ(result.nan_payload(), 7u);
}

TEST(DecimalTest, multiply_nan_propagates_payload_and_sign) {
  const auto left = -sourcemeta::core::Decimal::nan(9);
  const auto right = sourcemeta::core::Decimal{2};
  const auto result = left * right;
  EXPECT_TRUE(result.is_qnan());
  EXPECT_EQ(result.nan_payload(), 9u);
  EXPECT_TRUE(result.is_signed());
}

TEST(DecimalTest, divide_nan_propagates_payload) {
  const auto left = sourcemeta::core::Decimal{10};
  const auto right = sourcemeta::core::Decimal::nan(11);
  const auto result = left / right;
  EXPECT_TRUE(result.is_qnan());
  EXPECT_EQ(result.nan_payload(), 11u);
}

TEST(DecimalTest, remainder_nan_propagates_payload) {
  const auto left = sourcemeta::core::Decimal::nan(13);
  const auto right = sourcemeta::core::Decimal{4};
  const auto result = left % right;
  EXPECT_TRUE(result.is_qnan());
  EXPECT_EQ(result.nan_payload(), 13u);
}

TEST(DecimalTest, arithmetic_snan_becomes_qnan_with_payload_preserved) {
  const auto left = sourcemeta::core::Decimal::snan(21);
  const auto right = sourcemeta::core::Decimal{1};
  const auto result = left + right;
  EXPECT_TRUE(result.is_qnan());
  EXPECT_FALSE(result.is_snan());
  EXPECT_EQ(result.nan_payload(), 21u);
}

TEST(DecimalTest, to_int64_from_int64_min_constructor) {
  constexpr auto min_value = std::numeric_limits<std::int64_t>::min();
  const auto value = sourcemeta::core::Decimal{min_value};
  EXPECT_EQ(value.to_int64(), min_value);
}

TEST(DecimalTest, to_int64_int64_min_roundtrip_through_string) {
  constexpr auto min_value = std::numeric_limits<std::int64_t>::min();
  const auto value = sourcemeta::core::Decimal{"-9223372036854775808"};
  EXPECT_EQ(value.to_int64(), min_value);
}

TEST(DecimalTest, factory_nan_uint64_max_payload) {
  const auto value =
      sourcemeta::core::Decimal::nan(std::numeric_limits<std::uint64_t>::max());
  EXPECT_TRUE(value.is_nan());
  EXPECT_TRUE(value.is_qnan());
  EXPECT_EQ(value.nan_payload(), std::numeric_limits<std::uint64_t>::max());
}

TEST(DecimalTest, factory_snan_uint64_max_payload) {
  const auto value = sourcemeta::core::Decimal::snan(
      std::numeric_limits<std::uint64_t>::max());
  EXPECT_TRUE(value.is_nan());
  EXPECT_TRUE(value.is_snan());
  EXPECT_EQ(value.nan_payload(), std::numeric_limits<std::uint64_t>::max());
}

TEST(DecimalTest, factory_nan_above_int64_max_payload) {
  constexpr std::uint64_t payload{
      static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + 1};
  const auto value = sourcemeta::core::Decimal::nan(payload);
  EXPECT_TRUE(value.is_nan());
  EXPECT_EQ(value.nan_payload(), payload);
}

TEST(DecimalTest, parse_nan_uint64_max_payload) {
  const sourcemeta::core::Decimal value{"NaN18446744073709551615"};
  EXPECT_TRUE(value.is_nan());
  EXPECT_TRUE(value.is_qnan());
  EXPECT_EQ(value.nan_payload(), std::numeric_limits<std::uint64_t>::max());
}

TEST(DecimalTest, parse_snan_uint64_max_payload) {
  const sourcemeta::core::Decimal value{"sNaN18446744073709551615"};
  EXPECT_TRUE(value.is_nan());
  EXPECT_TRUE(value.is_snan());
  EXPECT_EQ(value.nan_payload(), std::numeric_limits<std::uint64_t>::max());
}

TEST(DecimalTest, format_nan_uint64_max_payload_round_trips) {
  const auto value =
      sourcemeta::core::Decimal::nan(std::numeric_limits<std::uint64_t>::max());
  EXPECT_EQ(value.to_string(), "NaN18446744073709551615");
}
