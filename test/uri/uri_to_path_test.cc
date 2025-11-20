#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/bar/baz"});
}

TEST(URI_to_path, unix_with_encoded_space) {
  const sourcemeta::core::URI uri{"file:///foo/My%20Folder/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/My Folder/file.txt"});
}

TEST(URI_to_path, unix_with_encoded_hash) {
  const sourcemeta::core::URI uri{"file:///foo/has%23hash"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/has#hash"});
}

TEST(URI_to_path, unix_with_encoded_question) {
  const sourcemeta::core::URI uri{"file:///foo/has%3Fquestion"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/has?question"});
}

TEST(URI_to_path, unix_with_encoded_percent) {
  const sourcemeta::core::URI uri{"file:///foo/50%25off"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/50%off"});
}

TEST(URI_to_path, unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/bar/"});
}

TEST(URI_to_path, unix_root) {
  const sourcemeta::core::URI uri{"file:///"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/"});
}

TEST(URI_to_path, windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"C:\\Program Files\\Test"});
}

TEST(URI_to_path, windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"c:\\temp\\logs"});
}

TEST(URI_to_path, windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"D:\\"});
}

TEST(URI_to_path, windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"C:\\foo\\bar\\"});
}

TEST(URI_to_path, windows_percent_and_plus) {
  const sourcemeta::core::URI uri{"file:///C:/path/50%25+plus.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"C:\\path\\50%+plus.txt"});
}

TEST(URI_to_path, windows_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"\\\\server\\share\\file.txt"});
}

TEST(URI_to_path, windows_unc_with_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"\\\\srv\\My Docs\\a b.txt"});
}

TEST(URI_to_path, unicode_unix) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{u8"/data/\u00e9clair.txt"});
}

TEST(URI_to_path, unicode_windows) {
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{u8"C:\\data\\r\u00e9sum\u00e9.doc"});
}

TEST(URI_to_path, non_file_uri_https) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, non_file_uri_http) {
  const sourcemeta::core::URI uri{"http://localhost:8080/api/v1"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/api/v1"});
}

TEST(URI_to_path, non_file_uri_no_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{});
}

TEST(URI_to_path, relative_uri) {
  const sourcemeta::core::URI uri{"foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"foo/bar/baz"});
}

TEST(URI_to_path, relative_uri_with_dot) {
  const sourcemeta::core::URI uri{"./foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"foo/bar"});
}

TEST(URI_to_path, empty_uri) {
  const sourcemeta::core::URI uri{""};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{});
}

TEST(URI_to_path, roundtrip_unix) {
  const std::filesystem::path original{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto roundtrip{uri.to_path()};
  EXPECT_EQ(roundtrip, original);
}

TEST(URI_to_path, roundtrip_unix_with_space) {
  const std::filesystem::path original{"/foo/My Folder/file.txt"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto roundtrip{uri.to_path()};
  EXPECT_EQ(roundtrip, original);
}

TEST(URI_to_path, roundtrip_unicode) {
  const std::filesystem::path original{u8"/data/\u00e9clair.txt"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto roundtrip{uri.to_path()};
  EXPECT_EQ(roundtrip, original);
}
