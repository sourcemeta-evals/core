#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

#include <filesystem>

// file:// UNIX paths

TEST(URI_to_path, file_unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar/baz"});
}

TEST(URI_to_path, file_unix_root) {
  const sourcemeta::core::URI uri{"file:///"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/"});
}

TEST(URI_to_path, file_unix_single_segment) {
  const sourcemeta::core::URI uri{"file:///foo"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo"});
}

TEST(URI_to_path, file_unix_with_encoded_space) {
  const sourcemeta::core::URI uri{"file:///foo/My%20Folder/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/My Folder/bar"});
}

TEST(URI_to_path, file_unix_with_encoded_hash) {
  const sourcemeta::core::URI uri{"file:///foo/has%23hash"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/has#hash"});
}

TEST(URI_to_path, file_unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar/"});
}

TEST(URI_to_path, file_unix_unicode) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{u8"/data/éclair.txt"});
}

// file:// Windows paths

TEST(URI_to_path, file_windows_drive) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"C:/Program Files/Test"});
}

TEST(URI_to_path, file_windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"c:/temp/logs"});
}

TEST(URI_to_path, file_windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"D:/"});
}

// file:// UNC paths

TEST(URI_to_path, file_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"//server/share/file.txt"});
}

TEST(URI_to_path, file_unc_with_encoded_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"//srv/My Docs/a b.txt"});
}

// file:// with empty authority (just file://)

TEST(URI_to_path, file_no_path) {
  const sourcemeta::core::URI uri{"file://"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/"});
}

// Non file:// URIs

TEST(URI_to_path, https_with_path) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, https_no_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{""});
}

TEST(URI_to_path, https_with_encoded_path) {
  const sourcemeta::core::URI uri{"https://example.com/foo%20bar/baz"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo bar/baz"});
}

TEST(URI_to_path, relative_path) {
  const sourcemeta::core::URI uri{"foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"foo/bar"});
}

TEST(URI_to_path, absolute_path_only) {
  const sourcemeta::core::URI uri{"/foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}
