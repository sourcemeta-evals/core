#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

#include <filesystem>

// file:// URIs - UNIX paths

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

TEST(URI_to_path, file_unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar/"});
}

TEST(URI_to_path, file_unix_percent_encoded_space) {
  const sourcemeta::core::URI uri{"file:///foo/My%20Folder/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/My Folder/bar"});
}

TEST(URI_to_path, file_unix_percent_encoded_hash) {
  const sourcemeta::core::URI uri{"file:///foo/has%23hash"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/has#hash"});
}

TEST(URI_to_path, file_unix_percent_encoded_question) {
  const sourcemeta::core::URI uri{"file:///foo/has%3Fquestion"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/has?question"});
}

TEST(URI_to_path, file_unix_percent_encoded_percent) {
  const sourcemeta::core::URI uri{"file:///foo/has%25percent"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/has%percent"});
}

TEST(URI_to_path, file_unix_unicode) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{u8"/data/\u00e9clair.txt"});
}

// file:// URIs - Windows paths

TEST(URI_to_path, file_windows_drive) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/C:/Program Files/Test"});
}

TEST(URI_to_path, file_windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/c:/temp/logs"});
}

TEST(URI_to_path, file_windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/D:/"});
}

// file:// URIs - UNC paths

TEST(URI_to_path, file_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"//server/share/file.txt"});
}

TEST(URI_to_path, file_unc_with_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"//srv/My Docs/a b.txt"});
}

// Non file:// URIs

TEST(URI_to_path, https_with_path) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, https_no_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{});
}

TEST(URI_to_path, https_root_path) {
  const sourcemeta::core::URI uri{"https://example.com/"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/"});
}

TEST(URI_to_path, relative_path) {
  const sourcemeta::core::URI uri{"foo/bar/baz"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"foo/bar/baz"});
}

TEST(URI_to_path, absolute_path_only) {
  const sourcemeta::core::URI uri{"/foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, empty_uri) {
  const sourcemeta::core::URI uri{""};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{});
}

// Roundtrip: from_path -> to_path

TEST(URI_to_path, roundtrip_unix_absolute) {
  const std::filesystem::path original{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  EXPECT_EQ(uri.to_path(), original);
}

TEST(URI_to_path, roundtrip_unix_with_space) {
  const std::filesystem::path original{"/foo/My Folder/bar"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  EXPECT_EQ(uri.to_path(), original);
}

TEST(URI_to_path, roundtrip_unix_trailing_slash) {
  const std::filesystem::path original{"/foo/bar/"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  EXPECT_EQ(uri.to_path(), original);
}
