#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <vector>

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

TEST(JSONPointer_json_auto, from_json_parsed_regex_token) {
  const auto document{sourcemeta::core::parse_json("\"/[\\\\-]/type\"")};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(document)};
  ASSERT_TRUE(result.has_value());
  const sourcemeta::core::Pointer expected{"[\\-]", "type"};
  EXPECT_EQ(result.value(), expected);
}

TEST(JSONPointer_json_auto, round_trip_control_character_tokens) {
  for (const auto &token : {std::string{"foo\nbar"}, std::string{"foo\tbar"}}) {
    const sourcemeta::core::Pointer pointer{token};
    const auto serialized{sourcemeta::core::to_json(pointer)};
    const auto back{
        sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};
    ASSERT_TRUE(back.has_value());
    EXPECT_EQ(back.value(), pointer);
  }
}

TEST(JSONPointer_json_auto, DISABLED_round_trip_nul_character_token) {
  const sourcemeta::core::Pointer pointer{std::string{"foo\0bar", 7}};
  const auto serialized{sourcemeta::core::to_json(pointer)};
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};
  ASSERT_TRUE(back.has_value());
  EXPECT_EQ(back.value(), pointer);
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
