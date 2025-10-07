#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/bar/baz");
}

TEST(URI_to_path, unix_with_space_and_reserved) {
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

TEST(URI_to_path, unix_root) {
  const sourcemeta::core::URI uri{"file:///"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/");
}

TEST(URI_to_path, unicode_unix) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/data/éclair.txt");
}

TEST(URI_to_path, windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "C:/Program Files/Test");
}

TEST(URI_to_path, windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "c:/temp/logs");
}

TEST(URI_to_path, windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "D:/");
}

TEST(URI_to_path, windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "C:/foo/bar/");
}

TEST(URI_to_path, windows_percent_and_plus) {
  const sourcemeta::core::URI uri{"file:///C:/path/50%25+plus.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "C:/path/50%+plus.txt");
}

TEST(URI_to_path, unicode_windows) {
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "C:/data/résumé.doc");
}

TEST(URI_to_path, windows_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "\\\\server/share/file.txt");
}

TEST(URI_to_path, windows_unc_with_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "\\\\srv/My Docs/a b.txt");
}

TEST(URI_to_path, https_with_path) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/bar/baz");
}

TEST(URI_to_path, https_with_encoded_path) {
  const sourcemeta::core::URI uri{"https://example.com/foo%20bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo bar/baz");
}

TEST(URI_to_path, http_root) {
  const sourcemeta::core::URI uri{"http://example.com/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/");
}

TEST(URI_to_path, http_no_path) {
  const sourcemeta::core::URI uri{"http://example.com"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "");
}

TEST(URI_to_path, relative_path) {
  const sourcemeta::core::URI uri{"../foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "../foo/bar");
}

TEST(URI_to_path, urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "example:schema");
}

TEST(URI_to_path, empty_uri) {
  const sourcemeta::core::URI uri{""};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "");
}

TEST(URI_to_path, fragment_only) {
  const sourcemeta::core::URI uri{"#fragment"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "");
}

TEST(URI_to_path, file_uri_no_path) {
  const sourcemeta::core::URI uri{"file://"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "");
}

TEST(URI_to_path, roundtrip_unix) {
  const std::filesystem::path original{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, roundtrip_windows) {
  const std::filesystem::path original{"C:/Program Files/Test"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result.string(), "C:/Program Files/Test");
}
