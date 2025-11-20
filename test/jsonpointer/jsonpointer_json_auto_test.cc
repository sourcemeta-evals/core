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
  // Test case for patternProperties with special characters like hyphens
  // This tests the fix for the separate compilation issue where JSON-encoded
  // strings were being double-encoded
  const sourcemeta::core::Pointer pointer{"patternProperties", "[\\-]"};
  const auto json_result{sourcemeta::core::to_json(pointer)};

  // Verify round-trip: to_json -> from_json should give us back the same
  // pointer
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(json_result)};
  EXPECT_TRUE(back.has_value());
  EXPECT_EQ(pointer, back.value());

  // Also test with a pre-serialized JSON string (simulating loaded compiled
  // schema)
  const sourcemeta::core::JSON preloaded{"/patternProperties/[\\-]"};
  const auto from_preloaded{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(preloaded)};
  EXPECT_TRUE(from_preloaded.has_value());
  EXPECT_EQ(pointer, from_preloaded.value());
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
