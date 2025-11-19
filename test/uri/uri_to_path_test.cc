#include <filesystem>
#include <gtest/gtest.h>
#include <sourcemeta/core/uri.h>

TEST(URI_to_path, unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/bar/baz");
}

TEST(URI_to_path, unix_with_spaces_and_encoded_chars) {
  const sourcemeta::core::URI uri{"file:///foo/My%20Folder/has%23hash"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/My Folder/has#hash");
}

TEST(URI_to_path, windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  const auto path{uri.to_path()};
  // On Linux, std::filesystem::path("C:/...") might be stored as is.
  // We check the string representation.
  EXPECT_EQ(path.string(), "C:/Program Files/Test");
}

TEST(URI_to_path, windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "c:/temp/logs");
}

TEST(URI_to_path, windows_unc) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "//server/share/file.txt");
}

TEST(URI_to_path, windows_unc_with_spaces) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "//srv/My Docs/file.txt");
}

TEST(URI_to_path, non_file_scheme) {
  const sourcemeta::core::URI uri{"http://example.com/foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/bar");
}

TEST(URI_to_path, non_file_scheme_encoded) {
  const sourcemeta::core::URI uri{"http://example.com/foo%20bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo bar");
}

TEST(URI_to_path, file_scheme_localhost) {
  const sourcemeta::core::URI uri{"file://localhost/foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/bar");
}

TEST(URI_to_path, empty_path) {
  const sourcemeta::core::URI uri{"file://"};
  const auto path{uri.to_path()};
  EXPECT_TRUE(path.empty());
}

TEST(URI_to_path, unicode_chars) {
  // %C3%A9 is é
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/data/éclair.txt");
}
