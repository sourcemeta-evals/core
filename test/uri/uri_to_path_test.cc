#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, non_file_returns_uri_path_component) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar%20baz"};
  EXPECT_EQ(uri.to_path().generic_string(), "/foo/bar%20baz");
}

TEST(URI_to_path, file_unix_path) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  EXPECT_EQ(uri.to_path().generic_string(), "/foo/bar/baz");
}

TEST(URI_to_path, file_unix_percent_decoding) {
  const sourcemeta::core::URI uri{
      "file:///foo/My%20Folder/has%23hash%3Fvalue%25"};
  EXPECT_EQ(uri.to_path().generic_string(), "/foo/My Folder/has#hash?value%");
}

TEST(URI_to_path, file_windows_drive_path) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  EXPECT_EQ(uri.to_path().generic_string(), "C:/Program Files/Test");
}

TEST(URI_to_path, file_windows_unc_path) {
  const sourcemeta::core::URI uri{"file://server/My%20Docs/a%20b.txt"};
  EXPECT_EQ(uri.to_path().generic_string(), "/server/My Docs/a b.txt");
}
