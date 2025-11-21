#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, file_unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/bar/baz"});
}

TEST(URI_to_path, file_unix_with_escaped_space) {
  const sourcemeta::core::URI uri{"file:///foo/My%20Folder/test"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/My Folder/test"});
}

TEST(URI_to_path, file_unix_with_escaped_special_chars) {
  const sourcemeta::core::URI uri{"file:///foo/has%23hash%3Fvalue%25"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/has#hash?value%"});
}

TEST(URI_to_path, file_unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/bar/"});
}

TEST(URI_to_path, file_unix_root) {
  const sourcemeta::core::URI uri{"file:///"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/"});
}

TEST(URI_to_path, file_windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"C:/Program Files/Test"});
}

TEST(URI_to_path, file_windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"c:/temp/logs"});
}

TEST(URI_to_path, file_windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"D:/"});
}

TEST(URI_to_path, file_windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"C:/foo/bar/"});
}

TEST(URI_to_path, file_windows_percent_and_plus) {
  const sourcemeta::core::URI uri{"file:///C:/path/50%25+plus.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"C:/path/50%+plus.txt"});
}

TEST(URI_to_path, file_windows_unc) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"//server/share/file.txt"});
}

TEST(URI_to_path, file_windows_unc_with_escaped_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"//srv/My Docs/a b.txt"});
}

TEST(URI_to_path, file_unicode_unix) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{u8"/data/éclair.txt"});
}

TEST(URI_to_path, file_unicode_windows) {
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{u8"C:/data/résumé.doc"});
}

TEST(URI_to_path, http_returns_path_component) {
  const sourcemeta::core::URI uri{"https://www.example.com/foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, http_with_query_returns_path_only) {
  const sourcemeta::core::URI uri{"https://www.example.com/foo?query=value"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo"});
}

TEST(URI_to_path, http_with_fragment_returns_path_only) {
  const sourcemeta::core::URI uri{"https://www.example.com/foo#fragment"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo"});
}

TEST(URI_to_path, relative_uri_returns_path) {
  const sourcemeta::core::URI uri{"foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"foo/bar/baz"});
}

TEST(URI_to_path, relative_uri_with_dot) {
  const sourcemeta::core::URI uri{"./foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"foo/bar"});
}

TEST(URI_to_path, relative_uri_with_dotdot) {
  const sourcemeta::core::URI uri{"../parent/dir"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"../parent/dir"});
}

TEST(URI_to_path, urn_returns_path) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"example:schema"});
}

TEST(URI_to_path, empty_uri_returns_empty_path) {
  const sourcemeta::core::URI uri{""};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{""});
}

TEST(URI_to_path, fragment_only_returns_empty_path) {
  const sourcemeta::core::URI uri{"#fragment"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{""});
}

TEST(URI_to_path, file_no_path_returns_empty) {
  const sourcemeta::core::URI uri{"file://"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{""});
}

TEST(URI_to_path, roundtrip_unix_absolute) {
  const std::filesystem::path original{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, roundtrip_unix_with_spaces) {
  const std::filesystem::path original{"/foo/My Folder/test"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, roundtrip_windows_drive) {
  const std::filesystem::path original{R"(C:\Program Files\Test)"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result.generic_string(), "C:/Program Files/Test");
}

TEST(URI_to_path, roundtrip_windows_unc) {
  const std::filesystem::path original{R"(\\server\share\file.txt)"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result.string(), "//server/share/file.txt");
}
