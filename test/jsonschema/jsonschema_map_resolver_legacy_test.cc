#include <gtest/gtest.h>
#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_SchemaMapResolver, embedded_resource_draft4) {
  sourcemeta::core::SchemaMapResolver resolver;

  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "id": "http://www.sourcemeta.com/test",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "definitions": {
      "string": {
        "id": "string",
        "type": "string"
      }
    }
  })JSON");

  const auto result{resolver.add(document)};
  EXPECT_TRUE(result);

  const sourcemeta::core::JSON embedded = sourcemeta::core::parse_json(R"JSON({
    "id": "http://www.sourcemeta.com/string",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "string"
  })JSON");

  EXPECT_TRUE(resolver("http://www.sourcemeta.com/test").has_value());
  EXPECT_TRUE(resolver("http://www.sourcemeta.com/string").has_value());
  EXPECT_EQ(resolver("http://www.sourcemeta.com/test").value(), document);
  EXPECT_EQ(resolver("http://www.sourcemeta.com/string").value(), embedded);
}
