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

TEST(JSONPointer_json_auto, from_json_escaped_characters) {
  // Test case for patternProperties with escaped characters like [\\-]
  // This reproduces the issue where JSON-escaped strings fail to parse
  const sourcemeta::core::Pointer expected_pointer{"patternProperties",
                                                   "[\\-]"};

  // Test with the actual serialized format that would come from templates
  // First serialize the pointer to see what format it produces
  const auto serialized{sourcemeta::core::to_json(expected_pointer)};

  // Then deserialize it back - this should work with our fix
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};

  // This should succeed after the fix
  EXPECT_TRUE(result.has_value());
  if (result.has_value()) {
    EXPECT_EQ(result.value(), expected_pointer);
  }
}

TEST(JSONPointer_json_auto, from_json_round_trip_with_escapes) {
  // Test round-trip serialization/deserialization with escaped characters
  const sourcemeta::core::Pointer original_pointer{"patternProperties",
                                                   "[\\-]"};

  // Serialize to JSON
  const auto serialized{sourcemeta::core::to_json(original_pointer)};

  // Deserialize back
  const auto deserialized{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};

  // Should round-trip correctly
  EXPECT_TRUE(deserialized.has_value());
  if (deserialized.has_value()) {
    EXPECT_EQ(deserialized.value(), original_pointer);
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
