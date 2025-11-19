#include <filesystem>
#include <gtest/gtest.h>
#include <sourcemeta/core/json_error.h>
#include <type_traits>

TEST(JSONFileParseError, path_returns_reference) {
  std::filesystem::path p{"/tmp/test.json"};
  sourcemeta::core::JSONFileParseError error{p, 1, 1, "error"};

  // Check if the return type is a const reference
  // The current implementation returns const std::filesystem::path (value)
  // We want const std::filesystem::path & (reference)
  using ReturnType = decltype(error.path());
  static_assert(std::is_same_v<ReturnType, const std::filesystem::path &>,
                "path() must return a const reference to avoid copying");
}
