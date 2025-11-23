#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/bar/baz");
}

TEST(URI_to_path, unix_with_escaped_space_and_reserved) {
  const sourcemeta::core::URI uri{
      "file:///foo/My%20Folder/has%23hash%3Fvalue%25"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/My Folder/has#hash?value%");
}

TEST(URI_to_path, unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/bar/");
}

TEST(URI_to_path, windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/C:/Program Files/Test");
}

TEST(URI_to_path, windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/c:/temp/logs");
}

TEST(URI_to_path, windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/D:/");
}

TEST(URI_to_path, windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/C:/foo/bar/");
}

TEST(URI_to_path, windows_percent_and_plus) {
  const sourcemeta::core::URI uri{"file:///C:/path/50%25+plus.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/C:/path/50%+plus.txt");
}

TEST(URI_to_path, windows_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "//server/share/file.txt");
}

TEST(URI_to_path, windows_unc_with_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "//srv/My Docs/a b.txt");
}

TEST(URI_to_path, unicode_unix) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/data/éclair.txt");
}

TEST(URI_to_path, unicode_windows) {
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/C:/data/résumé.doc");
}

TEST(URI_to_path, non_file_uri_http) {
  const sourcemeta::core::URI uri{"https://www.example.com/foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/bar");
}

TEST(URI_to_path, non_file_uri_ftp) {
  const sourcemeta::core::URI uri{"ftp://ftp.example.com/pub/files"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/pub/files");
}

TEST(URI_to_path, non_file_uri_with_escaped_chars) {
  const sourcemeta::core::URI uri{"https://example.com/path%20with%20spaces"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/path with spaces");
}

TEST(URI_to_path, relative_uri_with_path) {
  const sourcemeta::core::URI uri{"foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "foo/bar/baz");
}

TEST(URI_to_path, relative_uri_with_escaped_chars) {
  const sourcemeta::core::URI uri{"foo%20bar/baz%2Fqux"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "foo bar/baz/qux");
}

TEST(URI_to_path, uri_without_path) {
  const sourcemeta::core::URI uri{"https://www.example.com"};
  const auto path{uri.to_path()};
  EXPECT_TRUE(path.empty());
}

TEST(URI_to_path, empty_uri) {
  const sourcemeta::core::URI uri{""};
  const auto path{uri.to_path()};
  EXPECT_TRUE(path.empty());
}

TEST(URI_to_path, file_uri_root) {
  const sourcemeta::core::URI uri{"file:///"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/");
}

TEST(URI_to_path, roundtrip_unix_absolute) {
  const std::filesystem::path original{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, roundtrip_unix_with_special_chars) {
  const std::filesystem::path original{"/foo/My Folder/has#hash?value%"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, roundtrip_windows_drive) {
  const std::filesystem::path original{"/C:/Program Files/Test"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, roundtrip_windows_unc) {
  const std::filesystem::path original{"//server/share/file.txt"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, roundtrip_unicode) {
  const std::filesystem::path original{u8"/data/éclair.txt"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}
