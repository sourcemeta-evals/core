#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, file_unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar/baz"});
}

TEST(URI_to_path, file_unix_percent_decoded) {
  const sourcemeta::core::URI uri{
      "file:///foo/My%20Folder/has%23hash%3Fvalue%25"};
  EXPECT_EQ(uri.to_path(),
            std::filesystem::path{"/foo/My Folder/has#hash?value%"});
}

TEST(URI_to_path, file_windows_drive) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{R"(C:\Program Files\Test)"});
}

TEST(URI_to_path, file_windows_unc) {
  const sourcemeta::core::URI uri{"file://server/share/My%20Docs/a.txt"};
  EXPECT_EQ(uri.to_path(),
            std::filesystem::path{R"(\\server\share\My Docs\a.txt)"});
}

TEST(URI_to_path, non_file_uri_returns_path_component) {
  const sourcemeta::core::URI uri{"https://example.com/foo%20bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo%20bar"});
}

TEST(URI_to_path, relative_uri_returns_path_component) {
  const sourcemeta::core::URI uri{"../foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"../foo/bar"});
}

TEST(URI_to_path, missing_path_returns_empty_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  EXPECT_TRUE(uri.to_path().empty());
}
