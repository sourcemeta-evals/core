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

TEST(JSONSchema, broken_reference_error_throw) {
  static_assert(std::is_base_of_v<sourcemeta::core::SchemaReferenceError,
                                  sourcemeta::core::SchemaBrokenReferenceError>,
                "Must subclass SchemaReferenceError");
  static_assert(std::is_base_of_v<std::exception,
                                  sourcemeta::core::SchemaBrokenReferenceError>,
                "Must subclass std::exception");
  auto exception{sourcemeta::core::SchemaBrokenReferenceError(
      "https://example.com#/broken", sourcemeta::core::Pointer{"ref"},
      "Broken reference")};
  EXPECT_THROW(throw exception, sourcemeta::core::SchemaBrokenReferenceError);
  EXPECT_THROW(throw exception, sourcemeta::core::SchemaReferenceError);
  EXPECT_EQ(std::string{exception.what()}, "Broken reference");
  EXPECT_EQ(exception.id(), "https://example.com#/broken");
  EXPECT_EQ(sourcemeta::core::to_string(exception.location()), "/ref");
}
