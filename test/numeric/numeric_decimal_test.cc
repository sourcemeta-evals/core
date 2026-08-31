#include <gtest/gtest.h>

#include <sourcemeta/core/numeric.h>

using namespace std;

TEST(DecimalTest, minimal_compare_total) {
  const sourcemeta::core::Decimal a{1};
  EXPECT_EQ(a.compare_total(a), sourcemeta::core::Decimal{0});
}

TEST(DecimalTest, minimal_divide_integer) {
  EXPECT_EQ(
      sourcemeta::core::Decimal{6}.divide_integer(sourcemeta::core::Decimal{2}),
      sourcemeta::core::Decimal{3});
}

TEST(DecimalTest, minimal_logb) {
  EXPECT_EQ(sourcemeta::core::Decimal{100}.logb(),
            sourcemeta::core::Decimal{2});
}

TEST(DecimalTest, minimal_reduce) {
  EXPECT_EQ(sourcemeta::core::Decimal{"3.140"}.reduce(),
            sourcemeta::core::Decimal{"3.14"});
}

TEST(DecimalTest, minimal_same_quantum) {
  EXPECT_TRUE(sourcemeta::core::Decimal{"1.50"}.same_quantum(
      sourcemeta::core::Decimal{"2.50"}));
}

TEST(DecimalTest, minimal_scale_by) {
  const auto value =
      sourcemeta::core::Decimal{"1.5"}.scale_by(sourcemeta::core::Decimal{2});
  EXPECT_EQ(value, sourcemeta::core::Decimal{150});
}

TEST(DecimalTest, minimal_strict_from) {
  EXPECT_EQ(sourcemeta::core::Decimal::strict_from(0.5).to_string(), "0.5");
}
