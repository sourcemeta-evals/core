#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, file_unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/foo/bar/baz");
}

TEST(URI_to_path, file_unix_with_space_and_reserved) {
  const sourcemeta::core::URI uri{
      "file:///foo/My%20Folder/has%23hash%3Fvalue%25"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/foo/My Folder/has#hash?value%");
}

TEST(URI_to_path, file_unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/foo/bar/");
}

TEST(URI_to_path, file_windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "C:/Program Files/Test");
}

TEST(URI_to_path, file_windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "c:/temp/logs");
}

TEST(URI_to_path, file_windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "D:/");
}

TEST(URI_to_path, file_windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "C:/foo/bar/");
}

TEST(URI_to_path, file_windows_percent_and_plus) {
  const sourcemeta::core::URI uri{"file:///C:/path/50%25+plus.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "C:/path/50%+plus.txt");
}

TEST(URI_to_path, file_windows_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "//server/share/file.txt");
}

TEST(URI_to_path, file_windows_unc_with_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "//srv/My Docs/a b.txt");
}

TEST(URI_to_path, file_unicode_unix) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/data/éclair.txt");
}

TEST(URI_to_path, file_unicode_windows) {
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "C:/data/résumé.doc");
}

TEST(URI_to_path, non_file_https_with_path) {
  const sourcemeta::core::URI uri{"https://example.com/foo%20bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/foo%20bar");
}

TEST(URI_to_path, non_file_mailto_no_path) {
  const sourcemeta::core::URI uri{"mailto:joe@example.com"};
  const auto path{uri.to_path()};
  EXPECT_TRUE(path.generic_string().empty());
}

TEST(URI_to_path, fragment_only) {
  const sourcemeta::core::URI uri{"#frag"};
  const auto path{uri.to_path()};
  EXPECT_TRUE(path.generic_string().empty());
}

TEST(URI_to_path, file_localhost_treated_as_local) {
  const sourcemeta::core::URI uri{"file://localhost/foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/foo/bar");
}

TEST(URI_to_path, file_root_path) {
  const sourcemeta::core::URI uri{"file:///"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/");
}

TEST(URI_to_path, non_file_http_with_encoded_path) {
  const sourcemeta::core::URI uri{"http://example.com/path%2Fwith%2Fslashes"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/path%2Fwith%2Fslashes");
}

TEST(URI_to_path, file_unc_server_only) {
  const sourcemeta::core::URI uri{"file://server/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "//server/");
}

TEST(URI_to_path, file_empty_path) {
  const sourcemeta::core::URI uri{"file://"};
  const auto path{uri.to_path()};
  EXPECT_TRUE(path.generic_string().empty());
}
