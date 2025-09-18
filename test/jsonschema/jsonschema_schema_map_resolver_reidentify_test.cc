#include <gtest/gtest.h>

#include <optional>
#include <string>
#include <vector>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

using sourcemeta::core::JSON;

TEST(JSONSchema_SchemaMapResolver, reidentify_draft4_uses_id_keyword) {
  // Simple schema using draft-04 with an id
  JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "https://example.com/schema",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "object"
  })JSON");

  sourcemeta::core::SchemaMapResolver resolver;
  std::string registered_id;
  auto callback = [&](const JSON::String &id) { registered_id = id; };

  const bool added =
      resolver.add(document, std::nullopt, std::nullopt, callback);
  EXPECT_TRUE(added);
  ASSERT_FALSE(registered_id.empty());

  const auto stored = resolver(registered_id);
  ASSERT_TRUE(stored.has_value());
  const JSON &schema = stored.value();

  // Should use 'id' for draft-04 (not '$id')
  EXPECT_TRUE(schema.is_object());
  EXPECT_TRUE(schema.defines("$schema"));
  EXPECT_TRUE(schema.defines("id"));
  EXPECT_FALSE(schema.defines("$id"));
  EXPECT_EQ(schema.at("id").to_string(), registered_id);
}

TEST(JSONSchema_SchemaMapResolver, reidentify_2020_12_uses_dollar_id) {
  // Simple schema using 2020-12 with an $id
  JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com/root",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "type": "object"
  })JSON");

  sourcemeta::core::SchemaMapResolver resolver;
  std::string registered_id;
  auto callback = [&](const JSON::String &id) { registered_id = id; };

  const bool added =
      resolver.add(document, std::nullopt, std::nullopt, callback);
  EXPECT_TRUE(added);
  ASSERT_FALSE(registered_id.empty());

  const auto stored = resolver(registered_id);
  ASSERT_TRUE(stored.has_value());
  const JSON &schema = stored.value();

  // Should use '$id' in modern dialects
  EXPECT_TRUE(schema.is_object());
  EXPECT_TRUE(schema.defines("$schema"));
  EXPECT_TRUE(schema.defines("$id"));
  EXPECT_FALSE(schema.defines("id"));
  EXPECT_EQ(schema.at("$id").to_string(), registered_id);
}
