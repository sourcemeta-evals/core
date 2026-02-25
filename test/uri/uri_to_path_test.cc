#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

#include <filesystem>

TEST(URI_to_path, non_file_uri_returns_path_component) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, file_unix_absolute) {
  const sourcemeta::core::URI uri{"file:///tmp/My%20Folder/file.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/tmp/My Folder/file.txt"});
}

TEST(URI_to_path, file_windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"C:/Program Files/Test"});
}

TEST(URI_to_path, file_windows_unc) {
  const sourcemeta::core::URI uri{"file://server/My%20Docs/a%20b.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"//server/My Docs/a b.txt"});
}
