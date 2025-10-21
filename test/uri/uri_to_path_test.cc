#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/foo/bar/baz");
}

TEST(URI_to_path, unix_with_space_and_reserved) {
  const sourcemeta::core::URI uri{
      "file:///foo/My%20Folder/has%23hash%3Fvalue%25"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/foo/My Folder/has#hash?value%");
}

TEST(URI_to_path, unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/foo/bar/");
}

TEST(URI_to_path, windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "C:/Program Files/Test");
}

TEST(URI_to_path, windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "c:/temp/logs");
}

TEST(URI_to_path, windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "D:/");
}

TEST(URI_to_path, windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "C:/foo/bar/");
}

TEST(URI_to_path, windows_percent_and_plus) {
  const sourcemeta::core::URI uri{"file:///C:/path/50%25+plus.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "C:/path/50%+plus.txt");
}

TEST(URI_to_path, windows_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "\\\\server\\share\\file.txt");
}

TEST(URI_to_path, windows_unc_with_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "\\\\srv\\My Docs\\a b.txt");
}

TEST(URI_to_path, unicode_unix) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/data/\xC3\xA9"
                                   "clair.txt");
}

TEST(URI_to_path, unicode_windows) {
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "C:/data/r\xC3\xA9"
                                   "sum\xC3\xA9.doc");
}

TEST(URI_to_path, localhost_unix) {
  const sourcemeta::core::URI uri{"file://localhost/etc/hosts"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/etc/hosts");
}

TEST(URI_to_path, localhost_windows) {
  const sourcemeta::core::URI uri{"file://localhost/C:/Windows/System32"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "C:/Windows/System32");
}

TEST(URI_to_path, non_file_http) {
  const sourcemeta::core::URI uri{"https://example.com/api/v1?x=y#frag"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/api/v1");
}

TEST(URI_to_path, non_file_urn) {
  const sourcemeta::core::URI uri{"urn:example:something"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "example:something");
}

TEST(URI_to_path, fragment_only) {
  const sourcemeta::core::URI uri{"#frag"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "");
}

TEST(URI_to_path, empty_uri) {
  const sourcemeta::core::URI uri{""};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "");
}

TEST(URI_to_path, relative_path) {
  const sourcemeta::core::URI uri{"foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "foo/bar/baz");
}

TEST(URI_to_path, root_path) {
  const sourcemeta::core::URI uri{"/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/");
}

TEST(URI_to_path, file_root_path) {
  const sourcemeta::core::URI uri{"file:///"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/");
}

TEST(URI_to_path, roundtrip_unix) {
  const std::filesystem::path original{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result.generic_string(), original.generic_string());
}

TEST(URI_to_path, roundtrip_windows_drive) {
  const std::filesystem::path original{R"(C:\Program Files\Test)"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  const auto result_uri{sourcemeta::core::URI::from_path(result)};
  EXPECT_EQ(result_uri.recompose(), uri.recompose());
}

TEST(URI_to_path, roundtrip_windows_unc) {
  const std::filesystem::path original{R"(\\server\share\file.txt)"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  const auto result_uri{sourcemeta::core::URI::from_path(result)};
  EXPECT_EQ(result_uri.recompose(), uri.recompose());
}

TEST(URI_to_path, roundtrip_with_spaces) {
  const std::filesystem::path original{"/foo/My Folder/file.txt"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result.generic_string(), original.generic_string());
}

TEST(URI_to_path, roundtrip_unicode) {
  const std::filesystem::path original{"/data/\xC3\xA9clair.txt"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result.generic_string(), original.generic_string());
}
