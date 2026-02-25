#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, non_file_absolute) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  EXPECT_EQ(uri.to_path().string(), "/foo/bar");
}

TEST(URI_to_path, non_file_relative) {
  const sourcemeta::core::URI uri{"foo/bar"};
  EXPECT_EQ(uri.to_path().string(), "foo/bar");
}

TEST(URI_to_path, file_unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  EXPECT_EQ(uri.to_path().string(), "/foo/bar/baz");
}

TEST(URI_to_path, file_unix_percent_encoded) {
  const sourcemeta::core::URI uri{
      "file:///foo/My%20Folder/has%23hash%3Fvalue%25"};
  EXPECT_EQ(uri.to_path().string(), "/foo/My Folder/has#hash?value%");
}

TEST(URI_to_path, file_windows_drive) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  EXPECT_EQ(uri.to_path().string(), "C:/Program Files/Test");
}

TEST(URI_to_path, file_unc) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  EXPECT_EQ(uri.to_path().string(), "//server/share/file.txt");
}
