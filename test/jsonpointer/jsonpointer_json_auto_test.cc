#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

TEST(JSONPointer_json_auto, foo_bar_baz) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const auto result{sourcemeta::core::to_json(pointer)};
  const sourcemeta::core::JSON expected{"/foo/bar/baz"};
  EXPECT_EQ(result.size(), 12);
  EXPECT_EQ(result, expected);
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(pointer, back.value());
}

TEST(JSONPointer_json_auto, from_json_invalid_string) {
  const sourcemeta::core::JSON input{"x"};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSONPointer_json_auto, from_json_invalid_type) {
  const sourcemeta::core::JSON input{1};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSONPointer_json_auto, from_json_with_special_chars) {
  // Test case for the bug fix: from_json should use to_pointer(value)
  // instead of to_pointer(value.to_string()) to avoid double-encoding
  // This test uses a pointer with special characters that need escaping
  const sourcemeta::core::Pointer pointer{"pattern-properties", "[\\-]"};
  const auto json_value{sourcemeta::core::to_json(pointer)};

  // Deserialize back from JSON
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(json_value)};

  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), pointer);
  EXPECT_EQ(result.value().size(), 2);
  EXPECT_EQ(result.value().at(0).to_property(), "pattern-properties");
  EXPECT_EQ(result.value().at(1).to_property(), "[\\-]");
}

TEST(JSONWeakPointer_json_auto, to_json_foo_bar_baz) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};

  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};

  const auto result{sourcemeta::core::to_json(pointer)};
  const sourcemeta::core::JSON expected{"/foo/bar/baz"};
  EXPECT_EQ(result.size(), 12);
  EXPECT_EQ(result, expected);
}
