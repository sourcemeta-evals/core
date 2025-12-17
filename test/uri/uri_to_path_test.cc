#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar/baz"});
}

TEST(URI_to_path, unix_with_space_and_reserved) {
  const sourcemeta::core::URI uri{
      "file:///foo/My%20Folder/has%23hash%3Fvalue%25"};
  EXPECT_EQ(uri.to_path(),
            std::filesystem::path{"/foo/My Folder/has#hash?value%"});
}

TEST(URI_to_path, unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar/"});
}

TEST(URI_to_path, windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"C:/Program Files/Test"});
}

TEST(URI_to_path, windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"c:/temp/logs"});
}

TEST(URI_to_path, windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"D:/"});
}

TEST(URI_to_path, windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"C:/foo/bar/"});
}

TEST(URI_to_path, windows_percent_and_plus) {
  const sourcemeta::core::URI uri{"file:///C:/path/50%25+plus.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"C:/path/50%+plus.txt"});
}

TEST(URI_to_path, windows_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"\\\\server/share/file.txt"});
}

TEST(URI_to_path, windows_unc_with_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"\\\\srv/My Docs/a b.txt"});
}

TEST(URI_to_path, unicode_unix) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{u8"/data/éclair.txt"});
}

TEST(URI_to_path, unicode_windows) {
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{u8"C:/data/résumé.doc"});
}

TEST(URI_to_path, non_file_uri_https) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, non_file_uri_http) {
  const sourcemeta::core::URI uri{"http://localhost:8080/api/v1/resource"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/api/v1/resource"});
}

TEST(URI_to_path, non_file_uri_no_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{});
}

TEST(URI_to_path, relative_uri) {
  const sourcemeta::core::URI uri{"foo/bar/baz"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"foo/bar/baz"});
}

TEST(URI_to_path, relative_uri_with_dot) {
  const sourcemeta::core::URI uri{"./foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"foo/bar"});
}

TEST(URI_to_path, file_uri_root) {
  const sourcemeta::core::URI uri{"file:///"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/"});
}

TEST(URI_to_path, roundtrip_unix) {
  const std::filesystem::path original{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  EXPECT_EQ(uri.to_path(), original);
}

TEST(URI_to_path, roundtrip_unix_with_space) {
  const std::filesystem::path original{"/foo/My Folder/test"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  EXPECT_EQ(uri.to_path(), original);
}

TEST(URI_to_path, roundtrip_unicode) {
  const std::filesystem::path original{u8"/data/éclair.txt"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  EXPECT_EQ(uri.to_path(), original);
}
