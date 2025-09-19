#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include <exception>   // std::exception
#include <filesystem>  // std::filesystem::path
#include <string>      // std::string
#include <type_traits> // std::is_base_of_v
#include <utility>     // std::declval

static_assert(
    std::is_same_v<
        decltype(std::declval<const sourcemeta::core::JSONFileParseError &>()
                     .path()),
        const std::filesystem::path &>,
    "JSONFileParseError::path() must return const std::filesystem::path&");

TEST(JSON_error, parse_error) {
  static_assert(
      std::is_base_of_v<std::exception, sourcemeta::core::JSONParseError>,
      "Must subclass std::exception");
  auto exception{sourcemeta::core::JSONParseError(5, 6)};
  EXPECT_THROW(throw exception, sourcemeta::core::JSONParseError);
  EXPECT_EQ(std::string{exception.what()}, "Failed to parse the JSON document");
  EXPECT_EQ(exception.line(), 5);
  EXPECT_EQ(exception.column(), 6);
}
