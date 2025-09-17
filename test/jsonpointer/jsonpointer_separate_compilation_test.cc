#include <gtest/gtest.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

TEST(JSONPointer_separate_compilation, pattern_properties_with_escaped_chars) {
  // Test case for pattern properties with escaped characters like [\\-]
  const sourcemeta::core::Pointer pointer{"patternProperties", "[\\-]"};

  // Serialize to JSON (as would happen in separate compilation)
  const auto json_result = sourcemeta::core::to_json(pointer);
  EXPECT_TRUE(json_result.is_string());
  EXPECT_EQ(json_result.to_string(), "/patternProperties/[\\-]");

  // Deserialize from JSON (as would happen when loading compiled template)
  const auto deserialized =
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(json_result);
  EXPECT_TRUE(deserialized.has_value());
  EXPECT_EQ(deserialized.value(), pointer);
}

TEST(JSONPointer_separate_compilation,
     pattern_properties_with_double_backslash) {
  // Test case for pattern properties with double backslashes
  const sourcemeta::core::Pointer pointer{"patternProperties", "[\\\\-]"};

  // Serialize to JSON
  const auto json_result = sourcemeta::core::to_json(pointer);
  EXPECT_TRUE(json_result.is_string());

  // Deserialize from JSON
  const auto deserialized =
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(json_result);
  EXPECT_TRUE(deserialized.has_value());
  EXPECT_EQ(deserialized.value(), pointer);
}

TEST(JSONPointer_separate_compilation, json_encoded_vs_plain_string) {
  // Test the difference between JSON-encoded strings and plain strings
  const std::string pattern_with_backslash = "[\\-]";
  const sourcemeta::core::Pointer pointer{"patternProperties",
                                          pattern_with_backslash};

  // Create a JSON string that contains the JSON Pointer
  const sourcemeta::core::JSON json_pointer_string{
      sourcemeta::core::to_string(pointer)};

  // This should work correctly - deserializing from a JSON string
  const auto deserialized =
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(
          json_pointer_string);
  EXPECT_TRUE(deserialized.has_value());
  EXPECT_EQ(deserialized.value(), pointer);
}
