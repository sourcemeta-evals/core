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

TEST(JSONPointer_json_auto, pattern_properties_with_backslash) {
  // This simulates the patternProperties case with a backslash in the key
  // The pointer should be: /patternProperties/[\\-]
  const sourcemeta::core::Pointer pointer{"patternProperties", "[\\-]"};

  // Serialize to JSON
  const auto serialized{sourcemeta::core::to_json(pointer)};

  // Deserialize back
  const auto deserialized{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};

  EXPECT_TRUE(deserialized.has_value());
  EXPECT_EQ(pointer, deserialized.value());
}

TEST(JSONPointer_json_auto, pattern_properties_double_backslash) {
  // Test with double backslash which is common in regex patterns
  const sourcemeta::core::Pointer pointer{"patternProperties", "[\\\\-]"};

  // Serialize to JSON
  const auto serialized{sourcemeta::core::to_json(pointer)};

  // Deserialize back
  const auto deserialized{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};

  EXPECT_TRUE(deserialized.has_value());
  EXPECT_EQ(pointer, deserialized.value());
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
