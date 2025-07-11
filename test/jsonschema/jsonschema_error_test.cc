#include <gtest/gtest.h>

#include <sourcemeta/core/jsonschema.h>

#include <exception>   // std::exception
#include <string>      // std::string
#include <type_traits> // std::is_base_of_v

TEST(JSONSchema, schema_error_throw) {
  static_assert(
      std::is_base_of_v<std::exception, sourcemeta::core::SchemaError>,
      "Must subclass std::exception");
  auto exception{sourcemeta::core::SchemaError("My error")};
  EXPECT_THROW(throw exception, sourcemeta::core::SchemaError);
  EXPECT_EQ(std::string{exception.what()}, "My error");
}

TEST(JSONSchema, resolution_error_throw) {
  static_assert(std::is_base_of_v<std::exception,
                                  sourcemeta::core::SchemaResolutionError>,
                "Must subclass std::exception");
  auto exception{sourcemeta::core::SchemaResolutionError(
      "https://sourcemeta.com/test", "My error")};
  EXPECT_THROW(throw exception, sourcemeta::core::SchemaResolutionError);
  EXPECT_EQ(std::string{exception.what()}, "My error");
  EXPECT_EQ(exception.id(), "https://sourcemeta.com/test");
}

TEST(JSONSchema, unknown_dialect_error_throw) {
  static_assert(std::is_base_of_v<std::exception,
                                  sourcemeta::core::SchemaUnknownDialectError>,
                "Must subclass std::exception");
  auto exception{
      sourcemeta::core::SchemaUnknownDialectError("My dialect error")};
  EXPECT_THROW(throw exception, sourcemeta::core::SchemaUnknownDialectError);
  EXPECT_EQ(std::string{exception.what()}, "My dialect error");
}

TEST(JSONSchema, unknown_base_dialect_error_throw) {
  static_assert(
      std::is_base_of_v<std::exception,
                        sourcemeta::core::SchemaUnknownBaseDialectError>,
      "Must subclass std::exception");
  auto exception{
      sourcemeta::core::SchemaUnknownBaseDialectError("My base dialect error")};
  EXPECT_THROW(throw exception,
               sourcemeta::core::SchemaUnknownBaseDialectError);
  EXPECT_EQ(std::string{exception.what()}, "My base dialect error");
}
