#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include <exception>   // std::exception
#include <filesystem>  // std::filesystem::path
#include <string>      // std::string
#include <type_traits> // std::is_base_of_v, std::is_same_v

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

TEST(JSON_error, file_parse_error_path_is_const_reference) {
  const sourcemeta::core::JSONFileParseError exception{
      std::filesystem::path{"/foo/bar.json"}, 1, 2, "test error"};
  static_assert(
      std::is_same_v<decltype(exception.path()), const std::filesystem::path &>,
      "path() must return a const reference to avoid copies");
  EXPECT_EQ(exception.path(), std::filesystem::path{"/foo/bar.json"});
  EXPECT_EQ(&exception.path(), &exception.path());
}
