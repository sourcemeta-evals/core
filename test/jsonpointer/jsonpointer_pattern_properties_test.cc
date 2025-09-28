#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

TEST(JSONPointer_pattern_properties, from_json_with_escaped_hyphen) {
  const sourcemeta::core::JSON pointer_json{"/patternProperties/[\\\\-]"};

  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(pointer_json)};

  EXPECT_TRUE(result.has_value());
  if (result.has_value()) {
    const auto &pointer = result.value();
    EXPECT_EQ(pointer.size(), 2);
    EXPECT_TRUE(pointer.at(0).is_property());
    EXPECT_EQ(pointer.at(0).to_property(), "patternProperties");
    EXPECT_TRUE(pointer.at(1).is_property());
    EXPECT_EQ(pointer.at(1).to_property(), "[\\\\-]");
  }
}

TEST(JSONPointer_pattern_properties, round_trip_conversion) {
  const sourcemeta::core::Pointer original{"patternProperties", "[\\\\-]"};
  const auto json_result{sourcemeta::core::to_json(original)};
  const auto back_result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(json_result)};

  EXPECT_TRUE(back_result.has_value());
  if (back_result.has_value()) {
    EXPECT_EQ(original, back_result.value());
  }
}

TEST(JSONPointer_pattern_properties, minimal_reproduction_case) {
  const sourcemeta::core::JSON schema_pointer{
      "/patternProperties/[\\\\-]/type"};

  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(schema_pointer)};

  EXPECT_TRUE(result.has_value());
  if (result.has_value()) {
    const auto &pointer = result.value();
    EXPECT_EQ(pointer.size(), 3);
    EXPECT_TRUE(pointer.at(0).is_property());
    EXPECT_EQ(pointer.at(0).to_property(), "patternProperties");
    EXPECT_TRUE(pointer.at(1).is_property());
    EXPECT_EQ(pointer.at(1).to_property(), "[\\\\-]");
    EXPECT_TRUE(pointer.at(2).is_property());
    EXPECT_EQ(pointer.at(2).to_property(), "type");
  }
}

TEST(JSONPointer_pattern_properties, complex_escaped_patterns) {
  const sourcemeta::core::JSON pointer_json{
      "/patternProperties/[\\\\w\\\\-\\\\.]+"};

  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(pointer_json)};

  EXPECT_TRUE(result.has_value());
  if (result.has_value()) {
    const auto &pointer = result.value();
    EXPECT_EQ(pointer.size(), 2);
    EXPECT_TRUE(pointer.at(0).is_property());
    EXPECT_EQ(pointer.at(0).to_property(), "patternProperties");
    EXPECT_TRUE(pointer.at(1).is_property());
    EXPECT_EQ(pointer.at(1).to_property(), "[\\\\w\\\\-\\\\.]+");
  }
}
