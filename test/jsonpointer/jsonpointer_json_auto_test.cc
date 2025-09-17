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

TEST(JSONPointer_json_auto, from_json_escaped_characters) {
  const sourcemeta::core::JSON input{"/patternProperties/[\\\\-]/foo"};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(input)};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(result->size(), 3);
  EXPECT_TRUE(result->at(0).is_property());
  EXPECT_EQ(result->at(0).to_property(), "patternProperties");
  EXPECT_TRUE(result->at(1).is_property());
  EXPECT_EQ(result->at(1).to_property(), "[\\-]");
  EXPECT_TRUE(result->at(2).is_property());
  EXPECT_EQ(result->at(2).to_property(), "foo");
}

TEST(JSONPointer_json_auto, from_json_separate_compilation_scenario) {
  // Simulate the separate compilation scenario where a JSON Pointer
  // is serialized to JSON and then deserialized
  const sourcemeta::core::Pointer original_pointer{"patternProperties", "[\\-]",
                                                   "foo"};

  // Serialize to JSON (this is what separate compilation does)
  const auto serialized{sourcemeta::core::to_json(original_pointer)};
  EXPECT_TRUE(serialized.is_string());

  // Debug: Let's see what the serialized JSON looks like
  std::cout << "Serialized JSON: " << serialized.to_string() << std::endl;

  // Let's also try parsing the string directly to see if that works
  try {
    const auto direct_parse =
        sourcemeta::core::to_pointer(serialized.to_string());
    std::cout << "Direct parsing succeeded, size: " << direct_parse.size()
              << std::endl;
  } catch (const std::exception &e) {
    std::cout << "Direct parsing failed: " << e.what() << std::endl;
  }

  // Deserialize from JSON (this is where the bug occurs)
  const auto deserialized{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};
  EXPECT_TRUE(deserialized.has_value());
  EXPECT_EQ(deserialized->size(), 3);
  EXPECT_TRUE(deserialized->at(0).is_property());
  EXPECT_EQ(deserialized->at(0).to_property(), "patternProperties");
  EXPECT_TRUE(deserialized->at(1).is_property());
  EXPECT_EQ(deserialized->at(1).to_property(), "[\\-]");
  EXPECT_TRUE(deserialized->at(2).is_property());
  EXPECT_EQ(deserialized->at(2).to_property(), "foo");

  // Verify round-trip consistency
  EXPECT_EQ(original_pointer, deserialized.value());
}
