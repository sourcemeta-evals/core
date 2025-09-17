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
  const sourcemeta::core::Pointer pointer{"patternProperties", "[\\\\-]",
                                          "type"};
  const auto json_result{sourcemeta::core::to_json(pointer)};
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(json_result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(pointer, back.value());

  EXPECT_EQ(back.value().at(1).to_property(), "[\\\\-]");
}

TEST(JSONPointer_json_auto, from_json_regex_patterns) {
  const sourcemeta::core::Pointer pointer1{"patternProperties", "^[a-z\\\\d]+$",
                                           "properties"};
  const auto json1{sourcemeta::core::to_json(pointer1)};
  const auto back1{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(json1)};
  EXPECT_TRUE(back1.has_value());
  EXPECT_EQ(pointer1, back1.value());

  const sourcemeta::core::Pointer pointer2{"patternProperties", "[\\\\w\\\\-]+",
                                           "items"};
  const auto json2{sourcemeta::core::to_json(pointer2)};
  const auto back2{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(json2)};
  EXPECT_TRUE(back2.has_value());
  EXPECT_EQ(pointer2, back2.value());
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
