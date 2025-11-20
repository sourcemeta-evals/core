#include <filesystem>
#include <gtest/gtest.h>
#include <sourcemeta/core/json_error.h>

TEST(JSONFileParseErrorTest, Path) {
  std::filesystem::path p{"/tmp/test.json"};
  sourcemeta::core::JSONFileParseError error{p, 1, 1, "Error"};

  // Check that path() returns the correct path
  EXPECT_EQ(error.path(), p);

  // We want to verify that we can access the path without copy if we change the
  // signature
  const std::filesystem::path &ref = error.path();
  EXPECT_EQ(ref, p);
}
