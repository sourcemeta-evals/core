#include <gtest/gtest.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

TEST(SchemaMapResolverRefactor, AddDraft202012) {
  const auto schema = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com/root",
    "$defs": {
        "foo": {
            "$id": "https://example.com/foo"
        }
    }
})JSON");

  sourcemeta::core::SchemaMapResolver resolver;
  EXPECT_TRUE(resolver.add(schema));

  auto root = resolver("https://example.com/root");
  ASSERT_TRUE(root.has_value());
  EXPECT_TRUE(root->contains("$id"));
  EXPECT_FALSE(root->contains("id"));

  auto foo = resolver("https://example.com/foo");
  ASSERT_TRUE(foo.has_value());
  EXPECT_TRUE(foo->contains("$id"));
  EXPECT_FALSE(foo->contains("id"));
  EXPECT_EQ(foo->at("$id").to_string(), "https://example.com/foo");
}

TEST(SchemaMapResolverRefactor, AddDraft04) {
  const auto schema = sourcemeta::core::parse_json(R"JSON({
    "$schema": "http://json-schema.org/draft-04/schema#",
    "id": "http://example.com/root",
    "definitions": {
        "foo": {
            "id": "http://example.com/foo"
        }
    }
})JSON");

  sourcemeta::core::SchemaMapResolver resolver;
  EXPECT_TRUE(resolver.add(schema));

  auto root = resolver("http://example.com/root");
  ASSERT_TRUE(root.has_value());
  EXPECT_TRUE(root->contains("id"));
  EXPECT_FALSE(root->contains("$id"));

  auto foo = resolver("http://example.com/foo");
  ASSERT_TRUE(foo.has_value());
  EXPECT_TRUE(foo->contains("id"));
  EXPECT_FALSE(foo->contains("$id"));
  EXPECT_EQ(foo->at("id").to_string(), "http://example.com/foo");
}
