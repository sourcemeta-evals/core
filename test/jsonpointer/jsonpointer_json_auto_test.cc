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

TEST(JSONPointer_json_auto, from_json_with_backslashes) {
  // Test that backslashes in pointer tokens are preserved correctly
  // This is the case that was failing with patternProperties like "[\\-]"
  const sourcemeta::core::Pointer pointer{"patternProperties", "[\\-]"};
  const auto json_result{sourcemeta::core::to_json(pointer)};
  EXPECT_TRUE(json_result.is_string());

  // Deserialize back and verify the backslashes are preserved
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(json_result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(pointer, back.value());
  EXPECT_EQ(back.value().size(), 2);
  EXPECT_EQ(back.value().at(1).to_property(), "[\\-]");
}

TEST(JSONPointer_json_auto, from_json_regex_backslash_value) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "value": "/[\\-]/type"
  })JSON")};

  const auto result{sourcemeta::core::from_json<sourcemeta::core::Pointer>(
      input.at("value"))};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(sourcemeta::core::to_string(result.value()), "/[\\-]/type");
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
