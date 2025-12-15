#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{"/foo/bar/baz"});
}

TEST(URI_to_path, unix_with_space_and_reserved) {
  const sourcemeta::core::URI uri{
      "file:///foo/My%20Folder/has%23hash%3Fvalue%25"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{"/foo/My Folder/has#hash?value%"});
}

TEST(URI_to_path, unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{"/foo/bar/"});
}

TEST(URI_to_path, windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{R"(C:\Program Files\Test)"});
}

TEST(URI_to_path, windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{R"(c:\temp\logs)"});
}

TEST(URI_to_path, windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{R"(D:\)"});
}

TEST(URI_to_path, windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{R"(C:\foo\bar\)"});
}

TEST(URI_to_path, windows_percent_and_plus) {
  const sourcemeta::core::URI uri{"file:///C:/path/50%25+plus.txt"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{R"(C:\path\50%+plus.txt)"});
}

TEST(URI_to_path, windows_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{R"(\\server\share\file.txt)"});
}

TEST(URI_to_path, windows_unc_with_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{R"(\\srv\My Docs\a b.txt)"});
}

TEST(URI_to_path, unicode_unix) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{u8"/data/éclair.txt"});
}

TEST(URI_to_path, unicode_windows) {
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{u8R"(C:\data\résumé.doc)"});
}

TEST(URI_to_path, non_file_uri_https) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, non_file_uri_with_encoded_chars) {
  const sourcemeta::core::URI uri{"https://example.com/foo%20bar/baz%23qux"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{"/foo bar/baz#qux"});
}

TEST(URI_to_path, non_file_uri_no_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{});
}

TEST(URI_to_path, relative_uri_path) {
  const sourcemeta::core::URI uri{"foo/bar/baz"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{"foo/bar/baz"});
}

TEST(URI_to_path, file_uri_root) {
  const sourcemeta::core::URI uri{"file:///"};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{"/"});
}

TEST(URI_to_path, empty_uri) {
  const sourcemeta::core::URI uri{""};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, std::filesystem::path{});
}
