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

TEST(JSONPointer_json_auto, pattern_properties_case) {
  // Test case for the patternProperties separate compilation issue
  // This tests that JSON Pointers with special characters like those
  // used in regex patterns (e.g., "[\-]") are correctly serialized
  // and deserialized through from_json/to_json
  const sourcemeta::core::Pointer pointer{"patternProperties", "[\\-]", "type"};
  const auto serialized{sourcemeta::core::to_json(pointer)};
  const sourcemeta::core::JSON expected{"/patternProperties/[\\-]/type"};
  EXPECT_EQ(serialized, expected);

  // This is the critical test - deserializing back from JSON
  // The bug was that from_json was calling to_pointer(value.to_string())
  // which would double-parse the string, causing issues
  const auto deserialized{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};
  EXPECT_TRUE(deserialized.has_value());
  EXPECT_EQ(pointer, deserialized.value());
}

TEST(JSONPointer_json_auto, special_characters_roundtrip) {
  // Additional test for various special characters that might appear
  // in JSON Schema keywords or property names
  const sourcemeta::core::Pointer pointer{"properties", "foo-bar", "items"};
  const auto serialized{sourcemeta::core::to_json(pointer)};
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
