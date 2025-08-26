#include <gtest/gtest.h>
#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_errors, metaschema_unknown_dialect_object) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  EXPECT_THROW(
      (void)sourcemeta::core::metaschema(
          document, sourcemeta::core::schema_official_resolver, std::nullopt),
      sourcemeta::core::SchemaUnknownDialectError);
}

TEST(JSONSchema_errors, metaschema_unknown_dialect_boolean) {
  const sourcemeta::core::JSON document{true};
  EXPECT_THROW(
      (void)sourcemeta::core::metaschema(
          document, sourcemeta::core::schema_official_resolver, std::nullopt),
      sourcemeta::core::SchemaUnknownDialectError);
}

TEST(JSONSchema_errors, vocabularies_unknown_base_dialect) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  EXPECT_THROW(
      (void)sourcemeta::core::vocabularies(
          document, sourcemeta::core::schema_official_resolver, std::nullopt),
      sourcemeta::core::SchemaUnknownBaseDialectError);
}

TEST(JSONSchema_errors, reidentify_unknown_base_dialect) {
  sourcemeta::core::JSON document = sourcemeta::core::parse_json("{}");
  EXPECT_THROW(sourcemeta::core::reidentify(
                   document, "https://example.com/id",
                   sourcemeta::core::schema_official_resolver, std::nullopt),
               sourcemeta::core::SchemaUnknownBaseDialectError);
}
