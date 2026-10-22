#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <sstream>

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

TEST(JSONPointer_json_auto, from_json_regex_backslash_value) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "value": "/[\\-]/type"
  })JSON")};

  const auto result{sourcemeta::core::from_json<sourcemeta::core::Pointer>(
      input.at("value"))};
  EXPECT_TRUE(result.has_value());
  EXPECT_EQ(sourcemeta::core::to_string(result.value()), "/[\\-]/type");
}

TEST(JSONPointer_json_auto, round_trip_newline_token) {
  const sourcemeta::core::Pointer pointer{std::string{"foo\nbar"}};
  const auto serialized{sourcemeta::core::to_json(pointer)};
  const sourcemeta::core::JSON expected{"/foo\nbar"};
  EXPECT_EQ(serialized, expected);
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};
  ASSERT_TRUE(back.has_value());
  EXPECT_EQ(back.value(), pointer);
}

TEST(JSONPointer_json_auto, round_trip_tab_token) {
  const sourcemeta::core::Pointer pointer{std::string{"foo\tbar"}};
  const auto serialized{sourcemeta::core::to_json(pointer)};
  const sourcemeta::core::JSON expected{"/foo\tbar"};
  EXPECT_EQ(serialized, expected);
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};
  ASSERT_TRUE(back.has_value());
  EXPECT_EQ(back.value(), pointer);
}

TEST(JSONPointer_json_auto, round_trip_null_character_token) {
  const sourcemeta::core::Pointer pointer{std::string{"foo\0bar", 7}};
  const auto serialized{sourcemeta::core::to_json(pointer)};
  const sourcemeta::core::JSON expected{std::string{"/foo\0bar", 8}};
  EXPECT_EQ(serialized, expected);
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};
  ASSERT_TRUE(back.has_value());
  EXPECT_EQ(back.value(), pointer);
}

TEST(JSONPointer_json_auto, round_trip_control_character_token) {
  const sourcemeta::core::Pointer pointer{std::string{"foo\u0001bar"}};
  const auto serialized{sourcemeta::core::to_json(pointer)};
  const sourcemeta::core::JSON expected{"/foo\u0001bar"};
  EXPECT_EQ(serialized, expected);
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};
  ASSERT_TRUE(back.has_value());
  EXPECT_EQ(back.value(), pointer);
}

TEST(JSONPointer_json_auto, round_trip_quote_token) {
  const sourcemeta::core::Pointer pointer{std::string{"foo\"bar"}};
  const auto serialized{sourcemeta::core::to_json(pointer)};
  const sourcemeta::core::JSON expected{"/foo\"bar"};
  EXPECT_EQ(serialized, expected);
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};
  ASSERT_TRUE(back.has_value());
  EXPECT_EQ(back.value(), pointer);
}

TEST(JSONPointer_json_auto, round_trip_backslash_token) {
  const sourcemeta::core::Pointer pointer{std::string{"foo\\bar"}};
  const auto serialized{sourcemeta::core::to_json(pointer)};
  const sourcemeta::core::JSON expected{"/foo\\bar"};
  EXPECT_EQ(serialized, expected);
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};
  ASSERT_TRUE(back.has_value());
  EXPECT_EQ(back.value(), pointer);
}

TEST(JSONPointer_json_auto, round_trip_literal_escape_sequence_token) {
  const sourcemeta::core::Pointer pointer{std::string{"foo\\nbar"}};
  const auto serialized{sourcemeta::core::to_json(pointer)};
  const sourcemeta::core::JSON expected{"/foo\\nbar"};
  EXPECT_EQ(serialized, expected);
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};
  ASSERT_TRUE(back.has_value());
  EXPECT_EQ(back.value(), pointer);
}

TEST(JSONPointer_json_auto, round_trip_tilde_and_slash_tokens) {
  const sourcemeta::core::Pointer pointer{std::string{"~x"},
                                          std::string{"a/b"}};
  const auto serialized{sourcemeta::core::to_json(pointer)};
  const sourcemeta::core::JSON expected{"/~0x/a~1b"};
  EXPECT_EQ(serialized, expected);
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};
  ASSERT_TRUE(back.has_value());
  EXPECT_EQ(back.value(), pointer);
}

TEST(JSONPointer_json_auto, to_json_matches_parsed_text_newline) {
  const sourcemeta::core::Pointer pointer{std::string{"foo\nbar"}};
  const auto result{sourcemeta::core::to_json(pointer)};
  const auto expected{sourcemeta::core::parse_json(R"JSON("/foo\nbar")JSON")};
  EXPECT_EQ(result, expected);
}

TEST(JSONPointer_json_auto, from_json_parsed_text_newline) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "value": "/foo\nbar"
  })JSON")};

  const auto result{sourcemeta::core::from_json<sourcemeta::core::Pointer>(
      input.at("value"))};
  ASSERT_TRUE(result.has_value());
  const sourcemeta::core::Pointer expected{std::string{"foo\nbar"}};
  EXPECT_EQ(result.value(), expected);
}

TEST(JSONPointer_json_auto, from_json_parsed_text_quote_token) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "value": "/a\"b"
  })JSON")};

  const auto result{sourcemeta::core::from_json<sourcemeta::core::Pointer>(
      input.at("value"))};
  ASSERT_TRUE(result.has_value());
  const sourcemeta::core::Pointer expected{std::string{"a\"b"}};
  EXPECT_EQ(result.value(), expected);
}

TEST(JSONPointer_json_auto, from_json_parsed_text_literal_escaped_b) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "value": "/a\\b"
  })JSON")};

  const auto result{sourcemeta::core::from_json<sourcemeta::core::Pointer>(
      input.at("value"))};
  ASSERT_TRUE(result.has_value());
  const sourcemeta::core::Pointer expected{std::string{"a\\b"}};
  EXPECT_EQ(result.value(), expected);
}

TEST(JSONPointer_json_auto, from_json_parsed_text_literal_unicode_sequence) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "value": "/a\\u002Ab"
  })JSON")};

  const auto result{sourcemeta::core::from_json<sourcemeta::core::Pointer>(
      input.at("value"))};
  ASSERT_TRUE(result.has_value());
  const sourcemeta::core::Pointer expected{std::string{"a\\u002Ab"}};
  EXPECT_EQ(result.value(), expected);
}

TEST(JSONPointer_json_auto, round_trip_repeated_backslashes_token) {
  const sourcemeta::core::Pointer pointer{std::string{"a\\\\b"}};
  const auto serialized{sourcemeta::core::to_json(pointer)};
  const sourcemeta::core::JSON expected{"/a\\\\b"};
  EXPECT_EQ(serialized, expected);
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};
  ASSERT_TRUE(back.has_value());
  EXPECT_EQ(back.value(), pointer);
}

TEST(JSONPointer_json_auto, from_json_trailing_backslash_token) {
  const auto input{sourcemeta::core::parse_json(R"JSON({
    "value": "/a\\"
  })JSON")};

  const auto result{sourcemeta::core::from_json<sourcemeta::core::Pointer>(
      input.at("value"))};
  ASSERT_TRUE(result.has_value());
  const sourcemeta::core::Pointer expected{std::string{"a\\"}};
  EXPECT_EQ(result.value(), expected);
}

TEST(JSONPointer_json_auto, from_json_invalid_tilde_escape) {
  const sourcemeta::core::JSON input{"/~2"};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSONPointer_json_auto, from_json_trailing_tilde) {
  const sourcemeta::core::JSON input{"/~"};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(input)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSONPointer_json_auto, chain_serialise_reparse_backslash_token) {
  const sourcemeta::core::Pointer pointer{"[\\-]", "type"};
  const auto serialized{sourcemeta::core::to_json(pointer)};
  std::ostringstream stream;
  sourcemeta::core::stringify(serialized, stream);
  EXPECT_EQ(stream.str(), R"JSON("/[\\-]/type")JSON");
  const auto reparsed{sourcemeta::core::parse_json(stream.str())};
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(reparsed)};
  ASSERT_TRUE(back.has_value());
  EXPECT_EQ(back.value(), pointer);
}

TEST(JSONPointer_json_auto, chain_serialise_reparse_newline_token) {
  const sourcemeta::core::Pointer pointer{std::string{"foo\nbar"}};
  const auto serialized{sourcemeta::core::to_json(pointer)};
  std::ostringstream stream;
  sourcemeta::core::stringify(serialized, stream);
  EXPECT_EQ(stream.str(), R"JSON("/foo\nbar")JSON");
  const auto reparsed{sourcemeta::core::parse_json(stream.str())};
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(reparsed)};
  ASSERT_TRUE(back.has_value());
  EXPECT_EQ(back.value(), pointer);
}

TEST(JSONPointer_json_auto, chain_lookup_minimised_schema) {
  const auto schema{sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-07/schema#",
    "patternProperties": {
      "[\\-]": {
        "type": "string"
      }
    }
  })JSON")};

  const auto document{sourcemeta::core::parse_json(
      R"JSON("/patternProperties/[\\-]/type")JSON")};
  const auto pointer{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(document)};
  ASSERT_TRUE(pointer.has_value());
  EXPECT_EQ(sourcemeta::core::get(schema, pointer.value()),
            sourcemeta::core::JSON{"string"});
}

TEST(JSONPointer_json_auto, round_trip_empty_pointer) {
  const sourcemeta::core::Pointer pointer;
  const auto serialized{sourcemeta::core::to_json(pointer)};
  EXPECT_EQ(serialized, sourcemeta::core::JSON{""});
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};
  ASSERT_TRUE(back.has_value());
  EXPECT_EQ(back.value(), pointer);
}

TEST(JSONPointer_json_auto, round_trip_index_token) {
  const sourcemeta::core::Pointer pointer{"foo", 0};
  const auto serialized{sourcemeta::core::to_json(pointer)};
  EXPECT_EQ(serialized, sourcemeta::core::JSON{"/foo/0"});
  const auto back{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(serialized)};
  ASSERT_TRUE(back.has_value());
  EXPECT_EQ(back.value(), pointer);
}

TEST(JSONPointer_json_auto, from_json_backslash_token_then_invalid_tilde) {
  const auto document{sourcemeta::core::parse_json(R"JSON("/[\\-]/~2")JSON")};
  const auto result{
      sourcemeta::core::from_json<sourcemeta::core::Pointer>(document)};
  EXPECT_FALSE(result.has_value());
}

TEST(JSONPointer_json_auto, to_json_explicit_template_forms) {
  const sourcemeta::core::Pointer pointer{"[\\-]", "foo\nbar"};
  const sourcemeta::core::JSON expected{"/[\\-]/foo\nbar"};
  const auto result{sourcemeta::core::to_json<>(pointer)};
  EXPECT_EQ(result, expected);
  sourcemeta::core::JSON (*const callback)(const sourcemeta::core::Pointer &){
      &sourcemeta::core::to_json<sourcemeta::core::Pointer>};
  EXPECT_EQ(callback(pointer), expected);
}

TEST(JSONWeakPointer_json_auto, to_json_newline_token) {
  const std::string token{"foo\nbar"};
  const sourcemeta::core::WeakPointer pointer{std::cref(token)};
  const auto result{sourcemeta::core::to_json(pointer)};
  const sourcemeta::core::JSON expected{"/foo\nbar"};
  EXPECT_EQ(result, expected);
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

TEST(JSONWeakPointer_json_auto, to_json_explicit_template_forms) {
  const std::string regex{"[\\-]"};
  const std::string newline{"foo\nbar"};
  const sourcemeta::core::WeakPointer pointer{std::cref(regex),
                                              std::cref(newline)};
  const sourcemeta::core::JSON expected{"/[\\-]/foo\nbar"};
  const auto result{sourcemeta::core::to_json<>(pointer)};
  EXPECT_EQ(result, expected);
  sourcemeta::core::JSON (*const callback)(
      const sourcemeta::core::WeakPointer &){
      &sourcemeta::core::to_json<sourcemeta::core::WeakPointer>};
  EXPECT_EQ(callback(pointer), expected);
}
