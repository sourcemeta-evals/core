#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, non_file_uri_returns_path_component) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar%20baz"};
  EXPECT_EQ(uri.to_path().generic_string(), "/foo/bar%20baz");
}

TEST(URI_to_path, unix_file_uri) {
  const sourcemeta::core::URI uri{"file:///tmp/My%20Folder/file.txt"};
  EXPECT_EQ(uri.to_path().generic_string(), "/tmp/My Folder/file.txt");
}

TEST(URI_to_path, windows_drive_file_uri) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  EXPECT_EQ(uri.to_path().string(), R"(C:\Program Files\Test)");
}

TEST(URI_to_path, windows_unc_file_uri) {
  const sourcemeta::core::URI uri{"file://server/share/My%20Docs/file.txt"};
  EXPECT_EQ(uri.to_path().string(), R"(\\server\share\My Docs\file.txt)");
}

TEST(URI_to_path, empty_uri_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  EXPECT_TRUE(uri.to_path().empty());
}
