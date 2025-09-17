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

TEST(JSONPointer_json_auto, from_json_pattern_properties_escaped) {
  const sourcemeta::core::JSON input{"/patternProperties/[\\-]"};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(input)};
  EXPECT_TRUE(result.has_value());

  if (result.has_value()) {
    const auto &pointer = result.value();
    EXPECT_EQ(pointer.size(), 2);
    EXPECT_TRUE(pointer.at(0).is_property());
    EXPECT_EQ(pointer.at(0).to_property(), "patternProperties");
    EXPECT_TRUE(pointer.at(1).is_property());
    EXPECT_EQ(pointer.at(1).to_property(), "[\\-]");

    const auto back_to_json = sourcemeta::core::to_json(pointer);
    EXPECT_EQ(back_to_json, input);
  }
}

TEST(JSONPointer_json_auto, from_json_special_characters_roundtrip) {
  std::vector<std::string> test_cases = {
      "/patternProperties/[\\-]", "/patternProperties/^[a-z]+$",
      "/patternProperties/\\d+", "/properties/foo~0bar",
      "/properties/foo~1bar"};

  for (const auto &test_case : test_cases) {
    const sourcemeta::core::JSON input{test_case};
    const auto result{
        sourcemeta::core::from_json<sourcemeta::core::Pointer>(input)};
    EXPECT_TRUE(result.has_value()) << "Failed for: " << test_case;

    if (result.has_value()) {
      const auto back_to_json = sourcemeta::core::to_json(result.value());
      EXPECT_EQ(back_to_json, input) << "Round-trip failed for: " << test_case;
    }
  }
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
