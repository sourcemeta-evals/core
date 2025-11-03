#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, file_unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/bar/baz");
}

TEST(URI_to_path, file_unix_with_escaped_space) {
  const sourcemeta::core::URI uri{"file:///foo/My%20Folder/test"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/My Folder/test");
}

TEST(URI_to_path, file_unix_with_escaped_reserved) {
  const sourcemeta::core::URI uri{"file:///foo/has%23hash%3Fvalue%25"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/has#hash?value%");
}

TEST(URI_to_path, file_unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/bar/");
}

TEST(URI_to_path, file_windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/C:/Program Files/Test");
}

TEST(URI_to_path, file_windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/c:/temp/logs");
}

TEST(URI_to_path, file_windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/D:/");
}

TEST(URI_to_path, file_windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/C:/foo/bar/");
}

TEST(URI_to_path, file_windows_percent_and_plus) {
  const sourcemeta::core::URI uri{"file:///C:/path/50%25+plus.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/C:/path/50%+plus.txt");
}

TEST(URI_to_path, file_windows_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "//server/share/file.txt");
}

TEST(URI_to_path, file_windows_unc_with_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "//srv/My Docs/a b.txt");
}

TEST(URI_to_path, file_unicode_unix) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/data/éclair.txt");
}

TEST(URI_to_path, file_unicode_windows) {
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/C:/data/résumé.doc");
}

TEST(URI_to_path, non_file_http_returns_path_component) {
  const sourcemeta::core::URI uri{"https://www.example.com/foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/bar");
}

TEST(URI_to_path, non_file_https_with_query) {
  const sourcemeta::core::URI uri{"https://example.com/path?query=value"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/path");
}

TEST(URI_to_path, non_file_with_escaped_characters) {
  const sourcemeta::core::URI uri{"https://example.com/foo%20bar/baz%23test"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo bar/baz#test");
}

TEST(URI_to_path, relative_uri_with_path) {
  const sourcemeta::core::URI uri{"foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "foo/bar/baz");
}

TEST(URI_to_path, relative_uri_with_dot) {
  const sourcemeta::core::URI uri{"./foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "foo/bar");
}

TEST(URI_to_path, relative_uri_with_dotdot) {
  const sourcemeta::core::URI uri{"../parent/dir"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "../parent/dir");
}

TEST(URI_to_path, empty_uri) {
  const sourcemeta::core::URI uri{""};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "");
}

TEST(URI_to_path, fragment_only_uri) {
  const sourcemeta::core::URI uri{"#fragment"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "");
}

TEST(URI_to_path, root_path) {
  const sourcemeta::core::URI uri{"/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/");
}

TEST(URI_to_path, file_root_path) {
  const sourcemeta::core::URI uri{"file:///"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/");
}

TEST(URI_to_path, roundtrip_unix_absolute) {
  const std::filesystem::path original{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result.string(), original.string());
}

TEST(URI_to_path, roundtrip_unix_with_spaces) {
  const std::filesystem::path original{"/foo/My Folder/test"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result.string(), original.string());
}

TEST(URI_to_path, roundtrip_unix_trailing_slash) {
  const std::filesystem::path original{"/foo/bar/"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result.string(), original.string());
}
