#include <filesystem>
#include <gtest/gtest.h>
#include <sourcemeta/core/json_error.h>
#include <type_traits>
#include <utility>

using sourcemeta::core::JSONFileParseError;
using sourcemeta::core::JSONParseError;

static_assert(std::is_lvalue_reference_v<
                  decltype(std::declval<const JSONFileParseError &>().path())>,
              "JSONFileParseError::path() must return a const lvalue reference "
              "for lvalues");

TEST(JSON_file_parse_error_path, returns_reference_and_matches_member) {
  const std::filesystem::path p{"dummy.json"};
  const JSONParseError base{1, 1};
  const JSONFileParseError fe{p, base};
  const auto &ref = fe.path();
  EXPECT_EQ(ref, p);
  EXPECT_EQ(&ref, &fe.path());
}
