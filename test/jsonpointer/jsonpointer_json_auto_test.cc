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

TEST(JSONPointer_json_auto, from_json_regex_pattern_brackets) {
  const sourcemeta::core::JSON input{"/patternProperties/[\\-]/type"};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->size(), 3);
  EXPECT_TRUE(result->at(0).is_property());
  EXPECT_EQ(result->at(0).to_property(), "patternProperties");
  EXPECT_TRUE(result->at(1).is_property());
  EXPECT_EQ(result->at(1).to_property(), "[\\-]");
  EXPECT_TRUE(result->at(2).is_property());
  EXPECT_EQ(result->at(2).to_property(), "type");
}

TEST(JSONPointer_json_auto, from_json_regex_pattern_caret) {
  const sourcemeta::core::JSON input{"/patternProperties/^v/foo"};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->size(), 3);
  EXPECT_TRUE(result->at(0).is_property());
  EXPECT_EQ(result->at(0).to_property(), "patternProperties");
  EXPECT_TRUE(result->at(1).is_property());
  EXPECT_EQ(result->at(1).to_property(), "^v");
  EXPECT_TRUE(result->at(2).is_property());
  EXPECT_EQ(result->at(2).to_property(), "foo");
}

TEST(JSONPointer_json_auto, from_json_regex_pattern_backslash) {
  const sourcemeta::core::JSON input{"/patternProperties/\\\\d+/items"};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->size(), 3);
  EXPECT_TRUE(result->at(0).is_property());
  EXPECT_EQ(result->at(0).to_property(), "patternProperties");
  EXPECT_TRUE(result->at(1).is_property());
  EXPECT_EQ(result->at(1).to_property(), "\\\\d+");
  EXPECT_TRUE(result->at(2).is_property());
  EXPECT_EQ(result->at(2).to_property(), "items");
}

TEST(JSONPointer_json_auto, from_json_regex_pattern_complex) {
  const sourcemeta::core::JSON input{
      "/patternProperties/[a-zA-Z0-9_\\-]+/properties"};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->size(), 3);
  EXPECT_TRUE(result->at(0).is_property());
  EXPECT_EQ(result->at(0).to_property(), "patternProperties");
  EXPECT_TRUE(result->at(1).is_property());
  EXPECT_EQ(result->at(1).to_property(), "[a-zA-Z0-9_\\-]+");
  EXPECT_TRUE(result->at(2).is_property());
  EXPECT_EQ(result->at(2).to_property(), "properties");
}
