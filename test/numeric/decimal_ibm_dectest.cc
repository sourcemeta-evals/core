#include <gtest/gtest.h>

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/numeric.h>

#include <algorithm>   // std::transform, std::any_of
#include <cctype>      // std::tolower, std::isalnum
#include <filesystem>  // std::filesystem
#include <iostream>    // std::cerr
#include <sstream>     // std::istringstream
#include <stdexcept>   // std::invalid_argument, std::out_of_range
#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

static constexpr std::string_view SUPPORTED_OPERATIONS[] = {
    "compare",     "add",      "subtract",   "multiply",     "divide",
    "remainder",   "minus",    "plus",       "abs",          "tointegral",
    "tointegralx", "tosci",    "toeng",      "copy",         "copyabs",
    "copynegate",  "copysign", "comparesig", "comparetotal", "comparetotmag",
    "max",         "min",      "maxmag",     "minmag",       "divideint",
    "samequantum", "logb",     "scaleb",     "reduce",       "trim"};

static constexpr std::string_view UNARY_OPERATIONS[] = {
    "minus", "plus",   "abs",  "tointegral", "tointegralx",
    "tosci", "toeng",  "copy", "copyabs",    "copynegate",
    "logb",  "reduce", "trim"};

static constexpr std::string_view SKIP_CONDITIONS[] = {
    "inexact",   "rounded", "overflow",           "underflow",
    "subnormal", "clamped", "division_impossible"};

static constexpr std::string_view KNOWN_DIRECTIVES[] = {
    "precision", "rounding", "maxexponent", "minexponent",
    "extended",  "clamp",    "version",     "dectest"};

struct DecTestCase {
  std::string id;
  std::string operation;
  std::string operand1;
  std::string operand2;
  std::string expected;
  std::vector<std::string> conditions;
};

struct DecTestContext {
  int precision = 9;
  int max_exponent = 999;
  std::string rounding = "half_up";
};

static auto to_lower(std::string value) -> std::string {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char character) {
                   return static_cast<char>(std::tolower(character));
                 });
  return value;
}

static auto strip_quotes(const std::string &value) -> std::string {
  if (value.size() >= 2 && ((value.front() == '\'' && value.back() == '\'') ||
                            (value.front() == '"' && value.back() == '"'))) {
    return value.substr(1, value.size() - 2);
  }
  return value;
}

static auto has_condition(const std::vector<std::string> &conditions,
                          std::string_view name) -> bool {
  return std::any_of(
      conditions.begin(), conditions.end(),
      [name](const auto &condition) { return to_lower(condition) == name; });
}

static auto has_skip_condition(const std::vector<std::string> &conditions)
    -> bool {
  return std::any_of(
      std::begin(SKIP_CONDITIONS), std::end(SKIP_CONDITIONS),
      [&](const auto name) { return has_condition(conditions, name); });
}

static auto count_significant_digits(const std::string &value) -> std::size_t {
  const auto stripped{strip_quotes(value)};
  const auto lower{to_lower(stripped)};
  if (lower.find("nan") != std::string::npos ||
      lower.find("inf") != std::string::npos) {
    return 0;
  }

  std::size_t count{0};
  bool found_nonzero{false};
  bool in_exponent{false};
  for (const auto character : stripped) {
    if (character == 'e' || character == 'E') {
      in_exponent = true;
    } else if (!in_exponent &&
               std::isdigit(static_cast<unsigned char>(character))) {
      if (character != '0' || found_nonzero) {
        found_nonzero = true;
        count++;
      }
    }
  }
  return count;
}

static auto make_decimal(const std::string &raw) -> sourcemeta::core::Decimal {
  const auto value{strip_quotes(raw)};
  const auto lower{to_lower(value)};
  if (lower == "inf" || lower == "infinity" || lower.starts_with("+inf")) {
    return sourcemeta::core::Decimal::infinity();
  }
  if (lower.starts_with("-inf")) {
    return sourcemeta::core::Decimal::negative_infinity();
  }
  return sourcemeta::core::Decimal{value};
}

static auto decimal_abs(const sourcemeta::core::Decimal &value)
    -> sourcemeta::core::Decimal {
  if (value.is_nan() || value.is_snan()) {
    return value;
  }
  return value.is_signed() ? -value : value;
}

static auto decimal_minus(const sourcemeta::core::Decimal &value)
    -> sourcemeta::core::Decimal {
  if (value.is_nan() || value.is_snan()) {
    return value;
  }
  return sourcemeta::core::Decimal{0} - value;
}

static auto decimal_plus(const sourcemeta::core::Decimal &value)
    -> sourcemeta::core::Decimal {
  if (value.is_nan() || value.is_snan()) {
    return value;
  }
  return sourcemeta::core::Decimal{0} + value;
}

static auto decimal_copyabs(const sourcemeta::core::Decimal &value)
    -> sourcemeta::core::Decimal {
  if (value.is_snan()) {
    return sourcemeta::core::Decimal::snan(value.nan_payload());
  }
  if (value.is_nan()) {
    return sourcemeta::core::Decimal::nan(value.nan_payload());
  }
  return value.is_signed() ? -value : value;
}

static auto decimal_copynegate(const sourcemeta::core::Decimal &value)
    -> sourcemeta::core::Decimal {
  if (value.is_snan() || value.is_nan()) {
    const std::string prefix{value.is_signed() ? "" : "-"};
    const std::string kind{value.is_snan() ? "sNaN" : "NaN"};
    return sourcemeta::core::Decimal{prefix + kind +
                                     std::to_string(value.nan_payload())};
  }
  return -value;
}

static auto decimal_trim(const sourcemeta::core::Decimal &value)
    -> sourcemeta::core::Decimal {
  if (value.is_nan() || value.is_infinite()) {
    return value;
  }
  if (value.is_zero()) {
    return value.is_signed() ? sourcemeta::core::Decimal{"-0"}
                             : sourcemeta::core::Decimal{"0"};
  }

  // IBM General Decimal Arithmetic trim: strip trailing zeros from the
  // coefficient, but stop once the stored exponent reaches zero when the
  // operand had a fractional part. Operands with a non-negative stored
  // exponent strip freely like Decimal::reduce; operands with a negative
  // stored exponent stop once the fractional zeros are gone so trim(10.0) is
  // 10 (quantum E+0), not 1E+1. Recover the stored coefficient digits and
  // exponent via to_scientific_string's canonical form.
  const auto scientific = value.to_scientific_string();
  const auto e_pos = scientific.find('e');
  std::string coefficient_digits;
  for (std::size_t index = 0; index < e_pos; index++) {
    const auto character = scientific[index];
    if (character >= '0' && character <= '9') {
      coefficient_digits.push_back(character);
    }
  }
  const auto adjusted_exp = std::stoi(scientific.substr(e_pos + 1));
  const auto original_exp =
      adjusted_exp - static_cast<std::int32_t>(coefficient_digits.size()) + 1;

  if (original_exp == 0) {
    return value;
  }

  if (original_exp > 0) {
    return value.reduce();
  }

  std::size_t trailing_zeros = 0;
  while (trailing_zeros < coefficient_digits.size() &&
         coefficient_digits[coefficient_digits.size() - 1 - trailing_zeros] ==
             '0') {
    trailing_zeros++;
  }
  const auto max_strippable = -static_cast<std::int64_t>(original_exp);
  const auto strippable =
      std::min(static_cast<std::int64_t>(trailing_zeros), max_strippable);
  const auto new_exp = static_cast<std::int64_t>(original_exp) + strippable;
  const auto kept = coefficient_digits.substr(
      0, coefficient_digits.size() - static_cast<std::size_t>(strippable));
  const std::string sign_prefix = value.is_signed() ? "-" : "";
  return sourcemeta::core::Decimal{sign_prefix + kept + "e" +
                                   std::to_string(new_exp)};
}

TEST(DecimalTrimHelper, four_digit_coefficient_small_negative_exponent) {
  const sourcemeta::core::Decimal input{"1.2300e-30"};
  const sourcemeta::core::Decimal expected{"1.23e-30"};
  const auto result{decimal_trim(input)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.same_quantum(expected));
}

TEST(DecimalTrimHelper, three_digit_coefficient_small_negative_exponent) {
  const sourcemeta::core::Decimal input{"1.200e-30"};
  const sourcemeta::core::Decimal expected{"1.2e-30"};
  const auto result{decimal_trim(input)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.same_quantum(expected));
}

TEST(DecimalTrimHelper, trailing_zero_coefficient_no_decimal_small_fractional) {
  const sourcemeta::core::Decimal input{"1000e-10"};
  const sourcemeta::core::Decimal expected{"1e-7"};
  const auto result{decimal_trim(input)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.same_quantum(expected));
}

TEST(DecimalTrimHelper, trailing_zero_coefficient_very_small_fractional) {
  const sourcemeta::core::Decimal input{"100e-21"};
  const sourcemeta::core::Decimal expected{"1e-19"};
  const auto result{decimal_trim(input)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.same_quantum(expected));
}

TEST(DecimalTrimHelper, scientific_form_fractional_zeros) {
  const sourcemeta::core::Decimal input{"1.00e-7"};
  const sourcemeta::core::Decimal expected{"1e-7"};
  const auto result{decimal_trim(input)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.same_quantum(expected));
}

TEST(DecimalTrimHelper, negative_trailing_zero_coefficient) {
  const sourcemeta::core::Decimal input{"-1000e-10"};
  const sourcemeta::core::Decimal expected{"-1e-7"};
  const auto result{decimal_trim(input)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.same_quantum(expected));
  EXPECT_TRUE(result.is_signed());
}

TEST(DecimalTrimHelper, non_decimal_point_fractional_scientific_form) {
  const sourcemeta::core::Decimal input{"10E+1"};
  const sourcemeta::core::Decimal expected{"1e+2"};
  const auto result{decimal_trim(input)};
  EXPECT_EQ(result, expected);
  EXPECT_TRUE(result.same_quantum(expected));
}

static auto decimal_propagate_nan(const sourcemeta::core::Decimal &left,
                                  const sourcemeta::core::Decimal &right)
    -> sourcemeta::core::Decimal {
  const sourcemeta::core::Decimal *source;
  if (left.is_snan()) {
    source = &left;
  } else if (right.is_snan()) {
    source = &right;
  } else if (left.is_nan()) {
    source = &left;
  } else {
    source = &right;
  }

  auto result = sourcemeta::core::Decimal::nan(source->nan_payload());
  if (source->is_signed()) {
    result = -result;
  }
  return result;
}

static auto expect_comparison_result(const sourcemeta::core::Decimal &left,
                                     const sourcemeta::core::Decimal &right,
                                     const std::string &expected) -> bool {
  if (expected == "0") {
    EXPECT_TRUE(left == right);
  } else if (expected == "1") {
    EXPECT_TRUE(left > right);
  } else if (expected == "-1") {
    EXPECT_TRUE(left < right);
  } else {
    return false;
  }

  return true;
}

static auto expect_decimal_eq(const sourcemeta::core::Decimal &result,
                              const sourcemeta::core::Decimal &expected)
    -> void {
  if (expected.is_nan()) {
    EXPECT_TRUE(result.is_nan());
    if (expected.is_snan()) {
      EXPECT_TRUE(result.is_snan());
    } else {
      EXPECT_TRUE(result.is_qnan());
    }
    EXPECT_EQ(result.is_signed(), expected.is_signed());
    if (expected.nan_payload() < 1000) {
      EXPECT_EQ(result.nan_payload(), expected.nan_payload());
    }
  } else if (expected.is_infinite()) {
    EXPECT_TRUE(result.is_infinite());
    EXPECT_EQ(result.is_signed(), expected.is_signed());
  } else {
    EXPECT_EQ(result, expected);
    if (expected.is_zero()) {
      EXPECT_EQ(result.is_signed(), expected.is_signed());
    }
  }
}

class DecTest : public testing::Test {
public:
  explicit DecTest(DecTestCase test_case) : test_case_{std::move(test_case)} {}

  auto TestBody() -> void override {
    const auto operation{to_lower(this->test_case_.operation)};

    if (operation == "compare" || operation == "comparesig") {
      this->run_comparison(operation == "comparesig");
    } else if (operation == "add") {
      this->run_binary(
          [](const auto &left, const auto &right) { return left + right; });
    } else if (operation == "subtract") {
      this->run_binary(
          [](const auto &left, const auto &right) { return left - right; });
    } else if (operation == "multiply") {
      this->run_binary(
          [](const auto &left, const auto &right) { return left * right; });
    } else if (operation == "divide") {
      this->run_binary(
          [](const auto &left, const auto &right) { return left / right; });
    } else if (operation == "remainder") {
      this->run_binary(
          [](const auto &left, const auto &right) { return left % right; });
    } else if (operation == "minus") {
      this->run_unary([](const auto &value) { return decimal_minus(value); });
    } else if (operation == "copynegate") {
      this->run_unary(
          [](const auto &value) { return decimal_copynegate(value); });
    } else if (operation == "plus") {
      this->run_unary([](const auto &value) { return decimal_plus(value); });
    } else if (operation == "abs") {
      this->run_unary([](const auto &value) { return decimal_abs(value); });
    } else if (operation == "copyabs") {
      this->run_unary([](const auto &value) { return decimal_copyabs(value); });
    } else if (operation == "tointegral" || operation == "tointegralx") {
      this->run_unary([](const auto &value) { return value.to_integral(); });
    } else if (operation == "tosci" || operation == "toeng") {
      this->run_conversion();
    } else if (operation == "copy") {
      this->run_unary([](const auto &value) { return value; });
    } else if (operation == "copysign") {
      this->run_copysign();
    } else if (operation == "comparetotal") {
      this->run_binary([](const auto &left, const auto &right) {
        return left.compare_total(right);
      });
    } else if (operation == "comparetotmag") {
      this->run_binary([](const auto &left, const auto &right) {
        return decimal_copyabs(left).compare_total(decimal_copyabs(right));
      });
    } else if (operation == "max") {
      this->run_max_min(true, false);
    } else if (operation == "min") {
      this->run_max_min(false, false);
    } else if (operation == "maxmag") {
      this->run_max_min(true, true);
    } else if (operation == "minmag") {
      this->run_max_min(false, true);
    } else if (operation == "divideint") {
      this->run_binary([](const auto &left, const auto &right) {
        return left.divide_integer(right);
      });
    } else if (operation == "samequantum") {
      this->run_binary([](const auto &left, const auto &right) {
        return sourcemeta::core::Decimal{left.same_quantum(right) ? 1 : 0};
      });
    } else if (operation == "logb") {
      this->run_logb();
    } else if (operation == "scaleb") {
      this->run_binary([](const auto &left, const auto &right) {
        return left.scale_by(right);
      });
    } else if (operation == "reduce") {
      this->run_reduce();
    } else if (operation == "trim") {
      this->run_trim();
    } else {
      FAIL();
    }
  }

private:
  auto run_comparison(bool signal_on_nan) -> void {
    const auto left{make_decimal(this->test_case_.operand1)};
    const auto right{make_decimal(this->test_case_.operand2)};

    if (signal_on_nan &&
        has_condition(this->test_case_.conditions, "invalid_operation")) {
      EXPECT_TRUE(make_decimal(this->test_case_.expected).is_nan());
      return;
    }

    const auto expected{strip_quotes(this->test_case_.expected)};
    if (to_lower(expected).find("nan") != std::string::npos) {
      EXPECT_FALSE(left == right);
      EXPECT_TRUE(left != right);
      EXPECT_FALSE(left < right);
      EXPECT_FALSE(left > right);
      EXPECT_FALSE(left <= right);
      EXPECT_FALSE(left >= right);
      const auto expected_decimal{make_decimal(this->test_case_.expected)};
      EXPECT_TRUE(expected_decimal.is_nan());
      return;
    }

    EXPECT_TRUE(expect_comparison_result(left, right, expected));
  }

  template <typename Operation> auto run_binary(Operation op) -> void {
    const auto has_invalid{
        has_condition(this->test_case_.conditions, "invalid_operation") ||
        has_condition(this->test_case_.conditions, "division_undefined")};
    const auto has_divzero{
        has_condition(this->test_case_.conditions, "division_by_zero")};

    if (has_invalid) {
      try {
        const auto result{op(make_decimal(this->test_case_.operand1),
                             make_decimal(this->test_case_.operand2))};
        EXPECT_TRUE(result.is_nan());
      } catch (const sourcemeta::core::NumericInvalidOperationError &) {
        SUCCEED();
      } catch (const sourcemeta::core::NumericDivisionByZeroError &) {
        SUCCEED();
      } catch (const sourcemeta::core::NumericOverflowError &) {
        SUCCEED();
      }
      return;
    }

    if (has_divzero) {
      try {
        const auto result{op(make_decimal(this->test_case_.operand1),
                             make_decimal(this->test_case_.operand2))};
        const auto expected_value{make_decimal(this->test_case_.expected)};
        if (expected_value.is_infinite() && result.is_infinite()) {
          EXPECT_EQ(result.is_signed(), expected_value.is_signed());
        } else {
          FAIL();
        }
      } catch (const sourcemeta::core::NumericDivisionByZeroError &) {
        SUCCEED();
      }
      return;
    }

    const auto expected_value{make_decimal(this->test_case_.expected)};
    const auto result{op(make_decimal(this->test_case_.operand1),
                         make_decimal(this->test_case_.operand2))};
    expect_decimal_eq(result, expected_value);
  }

  template <typename Operation> auto run_unary(Operation op) -> void {
    if (has_condition(this->test_case_.conditions, "invalid_operation")) {
      try {
        EXPECT_TRUE(op(make_decimal(this->test_case_.operand1)).is_nan());
      } catch (const sourcemeta::core::NumericInvalidOperationError &) {
        SUCCEED();
      }
      return;
    }

    expect_decimal_eq(op(make_decimal(this->test_case_.operand1)),
                      make_decimal(this->test_case_.expected));
  }

  auto run_reduce() -> void {
    const auto input = make_decimal(this->test_case_.operand1);
    if (has_condition(this->test_case_.conditions, "invalid_operation")) {
      try {
        const auto result = input.reduce();
        expect_decimal_eq(result, make_decimal(this->test_case_.expected));
      } catch (const sourcemeta::core::NumericInvalidOperationError &) {
        SUCCEED();
      }
      return;
    }

    expect_decimal_eq(input.reduce(), make_decimal(this->test_case_.expected));
  }

  auto run_trim() -> void {
    const auto input = make_decimal(this->test_case_.operand1);
    const auto expected = make_decimal(this->test_case_.expected);
    const auto result = decimal_trim(input);
    expect_decimal_eq(result, expected);
    if (result.is_finite() && expected.is_finite()) {
      EXPECT_TRUE(result.same_quantum(expected))
          << "trim result quantum differs from expected";
    }
  }

  // TODO: Our to_scientific_string() always uses exponential form
  // (e.g. "1.0e+1" instead of "10"), which differs from the spec's
  // to-scientific-string. Our to_string() has the same issue with
  // to-engineering-string. We compare parsed values instead of strings.
  auto run_conversion() -> void {
    const auto input{strip_quotes(this->test_case_.operand1)};

    if (has_condition(this->test_case_.conditions, "conversion_syntax")) {
      try {
        const auto result{sourcemeta::core::Decimal{input}};
        EXPECT_TRUE(result.is_nan());
      } catch (const sourcemeta::core::DecimalParseError &) {
        SUCCEED();
      }
      return;
    }

    expect_decimal_eq(sourcemeta::core::Decimal{input},
                      make_decimal(this->test_case_.expected));
  }

  auto run_copysign() -> void {
    const auto left{make_decimal(this->test_case_.operand1)};
    const auto right{make_decimal(this->test_case_.operand2)};
    auto result = decimal_copyabs(left);
    if (right.is_signed()) {
      if (result.is_snan() || result.is_nan()) {
        const std::string kind{result.is_snan() ? "sNaN" : "NaN"};
        result = sourcemeta::core::Decimal{
            "-" + kind + std::to_string(result.nan_payload())};
      } else {
        result = -result;
      }
    }

    expect_decimal_eq(result, make_decimal(this->test_case_.expected));
  }

  auto run_logb() -> void {
    if (has_condition(this->test_case_.conditions, "division_by_zero")) {
      try {
        auto result = make_decimal(this->test_case_.operand1).logb();
        static_cast<void>(result);
        FAIL() << "Expected division_by_zero";
      } catch (const sourcemeta::core::NumericDivisionByZeroError &) {
        SUCCEED();
      }
      return;
    }

    this->run_unary([](const auto &value) { return value.logb(); });
  }

  auto run_max_min(bool is_max, bool by_magnitude) -> void {
    const auto left{make_decimal(this->test_case_.operand1)};
    const auto right{make_decimal(this->test_case_.operand2)};
    const auto expected{make_decimal(this->test_case_.expected)};

    if (left.is_snan() || right.is_snan()) {
      expect_decimal_eq(decimal_propagate_nan(left, right), expected);
      return;
    }

    if (left.is_qnan() && right.is_qnan()) {
      expect_decimal_eq(decimal_propagate_nan(left, right), expected);
      return;
    }

    if (left.is_qnan()) {
      expect_decimal_eq(right, expected);
      return;
    }

    if (right.is_qnan()) {
      expect_decimal_eq(left, expected);
      return;
    }

    int comparison;
    if (by_magnitude) {
      auto abs_left = decimal_abs(left);
      auto abs_right = decimal_abs(right);
      comparison = abs_left < abs_right ? -1 : (abs_left > abs_right ? 1 : 0);
    } else {
      comparison = left < right ? -1 : (left > right ? 1 : 0);
    }

    sourcemeta::core::Decimal result{0};
    if (comparison != 0) {
      result = (is_max == (comparison > 0)) ? left : right;
    } else {
      auto total_cmp = left.compare_total(right);
      result =
          (is_max == (total_cmp > sourcemeta::core::Decimal{0})) ? left : right;
    }

    expect_decimal_eq(result, expected);
  }

  DecTestCase test_case_;
};

// ---------------------------------------------------------------------------
// Parsing
// ---------------------------------------------------------------------------

static auto is_supported_operation(const std::string &operation) -> bool {
  const auto lower{to_lower(operation)};
  return std::any_of(std::begin(SUPPORTED_OPERATIONS),
                     std::end(SUPPORTED_OPERATIONS),
                     [&lower](const auto name) { return lower == name; });
}

static auto is_unary_operation(std::string_view operation) -> bool {
  return std::any_of(
      std::begin(UNARY_OPERATIONS), std::end(UNARY_OPERATIONS),
      [operation](const auto name) { return operation == name; });
}

static auto read_quoted_token(std::istringstream &stream, std::string &token)
    -> bool {
  if (!(stream >> token)) {
    return false;
  }
  if (!token.empty() && (token.front() == '\'' || token.front() == '"')) {
    const char quote{token.front()};
    while (token.back() != quote || token.size() == 1) {
      std::string next;
      if (!(stream >> next)) {
        return false;
      }
      token += " " + next;
    }
  }
  return true;
}

static auto parse_test_line(const std::string &line, DecTestCase &test_case)
    -> bool {
  std::istringstream stream{line};
  std::string token;

  if (!(stream >> test_case.id) || !(stream >> test_case.operation)) {
    return false;
  }
  if (!is_supported_operation(test_case.operation)) {
    return false;
  }
  if (!read_quoted_token(stream, test_case.operand1)) {
    return false;
  }

  test_case.operand2.clear();
  test_case.conditions.clear();

  if (is_unary_operation(to_lower(test_case.operation))) {
    if (!(stream >> token) || token != "->") {
      return false;
    }
  } else {
    if (!read_quoted_token(stream, test_case.operand2)) {
      return false;
    }
    if (test_case.operand2 == "->") {
      test_case.operand2.clear();
    } else if (!(stream >> token) || token != "->") {
      return false;
    }
  }

  if (!read_quoted_token(stream, test_case.expected)) {
    return false;
  }

  while (stream >> token) {
    test_case.conditions.push_back(std::move(token));
  }

  return true;
}

static auto parse_directive(const std::string &line, DecTestContext &context)
    -> bool {
  const auto colon_position{line.find(':')};
  if (colon_position == std::string::npos) {
    return false;
  }

  const std::string_view full_line{line};
  auto key{full_line.substr(0, colon_position)};
  auto value{full_line.substr(colon_position + 1)};

  while (!key.empty() &&
         (key.back() == ' ' || key.back() == '\t' || key.back() == '\r')) {
    key.remove_suffix(1);
  }
  while (!value.empty() && (value.front() == ' ' || value.front() == '\t')) {
    value.remove_prefix(1);
  }
  while (!value.empty() && (value.back() == ' ' || value.back() == '\t' ||
                            value.back() == '\r')) {
    value.remove_suffix(1);
  }

  const auto lower_key{to_lower(std::string{key})};
  if (lower_key == "precision") {
    context.precision = std::stoi(std::string{value});
    return true;
  }
  if (lower_key == "rounding") {
    context.rounding = to_lower(std::string{value});
    return true;
  }
  if (lower_key == "maxexponent") {
    context.max_exponent = std::stoi(std::string{value});
    return true;
  }

  return std::any_of(
      std::begin(KNOWN_DIRECTIVES), std::end(KNOWN_DIRECTIVES),
      [&lower_key](const auto name) { return lower_key == name; });
}

// ---------------------------------------------------------------------------
// Skip logic
// ---------------------------------------------------------------------------

static auto should_skip_file(const std::string &filename) -> bool {
  const auto lower{to_lower(filename)};
  if (lower.starts_with("dd") || lower.starts_with("dq") ||
      lower.starts_with("ds")) {
    return true;
  }
  return lower.find("randoms") != std::string::npos ||
         lower.find("randombound") != std::string::npos ||
         lower == "testall.dectest" || lower == "inexact.dectest" ||
         lower == "rounding.dectest" || lower == "powersqrt.dectest";
}

static auto should_skip_test(const DecTestCase &test_case,
                             const DecTestContext &context) -> bool {
  const auto operation{to_lower(test_case.operation)};

  if (test_case.operand1.find('#') != std::string::npos ||
      test_case.operand2.find('#') != std::string::npos ||
      test_case.expected.find('#') != std::string::npos) {
    return true;
  }

  if (operation == "compare" || operation == "comparetotal" ||
      operation == "comparetotmag" || operation == "comparesig" ||
      operation == "copy" || operation == "copyabs" ||
      operation == "copynegate" || operation == "copysign" ||
      operation == "samequantum") {
    return false;
  }

  if (operation == "divideint" || operation == "reduce" ||
      operation == "trim" || operation == "scaleb") {
    if (operation == "scaleb" &&
        has_condition(test_case.conditions, "invalid_operation")) {
      auto scale_bound = 2 * (context.max_exponent + context.precision);
      try {
        auto scale_value = std::stoll(test_case.operand2);
        if (scale_value > scale_bound || scale_value < -scale_bound) {
          return true;
        }
      } catch (const std::invalid_argument &) {
        return true;
      } catch (const std::out_of_range &) {
        return true;
      }
    }

    return has_skip_condition(test_case.conditions);
  }

  // TODO: Our Decimal context is fixed at 16-digit precision. Tests
  // expecting results with more significant digits would produce
  // different (truncated) results in our context.
  if (count_significant_digits(test_case.expected) > 16) {
    return true;
  }

  // TODO: Our Decimal context uses half_even rounding exclusively.
  // tointegral results depend on rounding mode, so we can only run
  // these tests when the file's rounding directive matches ours.
  if (operation == "tointegral" || operation == "tointegralx") {
    if (context.rounding != "half_even") {
      return true;
    }
  }

  // IEEE 754 §6.3: under round-toward-negative (floor), the sign of a zero
  // sum or difference is '-' where every other rounding attribute gives '+'.
  // Our Decimal has no rounding mode knob, so zero-result add/subtract rows
  // under floor rounding are skipped.
  if ((operation == "add" || operation == "subtract") &&
      context.rounding == "floor" &&
      count_significant_digits(test_case.expected) == 0) {
    return true;
  }

  return has_skip_condition(test_case.conditions);
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

static auto sanitize_test_name(const std::string &name) -> std::string {
  std::string result;
  result.reserve(name.size());
  std::transform(name.begin(), name.end(), std::back_inserter(result),
                 [](unsigned char character) -> char {
                   return std::isalnum(character) || character == '_'
                              ? static_cast<char>(character)
                              : '_';
                 });
  return result;
}

auto main(int argc, char **argv) -> int {
  testing::InitGoogleTest(&argc, argv);
  const std::filesystem::path dectest_path{DECTEST_PATH};

  std::size_t total_registered{0};
  std::size_t total_skipped{0};

  for (const auto &entry : std::filesystem::directory_iterator(dectest_path)) {
    if (!entry.is_regular_file()) {
      continue;
    }

    const auto filepath{entry.path()};
    if (filepath.extension() != ".decTest") {
      continue;
    }
    if (should_skip_file(filepath.filename().string())) {
      continue;
    }

    const auto suite_name{"DecTest_" +
                          sanitize_test_name(filepath.stem().string())};

    auto file{sourcemeta::core::read_file(filepath)};
    DecTestContext context;
    std::string line;

    while (std::getline(file, line)) {
      if (line.empty() || line.starts_with("--")) {
        continue;
      }

      auto start{line.find_first_not_of(" \t")};
      if (start == std::string::npos) {
        continue;
      }
      line = line.substr(start);

      if (parse_directive(line, context)) {
        continue;
      }

      DecTestCase test_case;
      if (!parse_test_line(line, test_case)) {
        continue;
      }

      if (should_skip_test(test_case, context)) {
        total_skipped++;
        continue;
      }

      const auto test_name{sanitize_test_name(test_case.id)};
      testing::RegisterTest(suite_name.c_str(), test_name.c_str(), nullptr,
                            nullptr, __FILE__, __LINE__,
                            [test_case = std::move(test_case)]() -> DecTest * {
                              return new DecTest(test_case);
                            });
      total_registered++;
    }
  }

  std::cerr << "DecTest: registered " << total_registered << " tests, skipped "
            << total_skipped << "\n";

  if (total_registered == 0) {
    std::cerr << "ERROR: No tests were registered!\n";
    return 1;
  }

  return RUN_ALL_TESTS();
}
