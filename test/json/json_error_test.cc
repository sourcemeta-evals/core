#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>

#include <exception>   // std::exception
#include <filesystem>  // std::filesystem::path
#include <string>      // std::string
#include <type_traits> // std::is_base_of_v

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

TEST(JSON_error, file_parse_error_path_no_copy) {
  const std::filesystem::path file_path{"/tmp/test.json"};
  const sourcemeta::core::JSONFileParseError exception{file_path, 1, 2,
                                                       "Test error"};
  const std::filesystem::path &first_call = exception.path();
  const std::filesystem::path &second_call = exception.path();
  EXPECT_EQ(&first_call, &second_call);
}
