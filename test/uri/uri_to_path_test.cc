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
#if defined(_WIN32)
  EXPECT_EQ(path, std::filesystem::path{R"(C:\Program Files\Test)"});
#else
  EXPECT_EQ(path, std::filesystem::path{"/C:/Program Files/Test"});
#endif
}

TEST(URI_to_path, file_windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const auto path{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(path, std::filesystem::path{R"(c:\temp\logs)"});
#else
  EXPECT_EQ(path, std::filesystem::path{"/c:/temp/logs"});
#endif
}

TEST(URI_to_path, file_windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const auto path{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(path, std::filesystem::path{R"(D:\)"});
#else
  EXPECT_EQ(path, std::filesystem::path{"/D:/"});
#endif
}

TEST(URI_to_path, file_windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  const auto path{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(path, std::filesystem::path{R"(C:\foo\bar\)"});
#else
  EXPECT_EQ(path, std::filesystem::path{"/C:/foo/bar/"});
#endif
}

TEST(URI_to_path, file_windows_with_percent_and_plus) {
  const sourcemeta::core::URI uri{"file:///C:/path/50%25+plus.txt"};
  const auto path{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(path, std::filesystem::path{R"(C:\path\50%+plus.txt)"});
#else
  EXPECT_EQ(path, std::filesystem::path{"/C:/path/50%+plus.txt"});
#endif
}

TEST(URI_to_path, file_windows_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto path{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(path, std::filesystem::path{R"(\\server\share\file.txt)"});
#else
  EXPECT_EQ(path, std::filesystem::path{"//server/share/file.txt"});
#endif
}

TEST(URI_to_path, file_windows_unc_with_escaped_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  const auto path{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(path, std::filesystem::path{R"(\\srv\My Docs\a b.txt)"});
#else
  EXPECT_EQ(path, std::filesystem::path{"//srv/My Docs/a b.txt"});
#endif
}

TEST(URI_to_path, file_unicode_escaped) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{u8"/data/éclair.txt"});
}

TEST(URI_to_path, file_unicode_escaped_windows) {
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  const auto path{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(path, std::filesystem::path{u8R"(C:\data\résumé.doc)"});
#else
  EXPECT_EQ(path, std::filesystem::path{u8"/C:/data/résumé.doc"});
#endif
}

TEST(URI_to_path, non_file_http_returns_path_component) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, non_file_https_returns_path_component) {
  const sourcemeta::core::URI uri{"https://www.example.com/path/to/resource"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/path/to/resource"});
}

TEST(URI_to_path, non_file_ftp_returns_path_component) {
  const sourcemeta::core::URI uri{"ftp://ftp.example.com/pub/files"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/pub/files"});
}

TEST(URI_to_path, non_file_no_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{});
}

TEST(URI_to_path, relative_uri_with_path) {
  const sourcemeta::core::URI uri{"foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"foo/bar/baz"});
}

TEST(URI_to_path, relative_uri_single_segment) {
  const sourcemeta::core::URI uri{"file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"file.txt"});
}

TEST(URI_to_path, empty_uri) {
  const sourcemeta::core::URI uri{""};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{});
}

TEST(URI_to_path, fragment_only) {
  const sourcemeta::core::URI uri{"#fragment"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{});
}

TEST(URI_to_path, file_empty_path) {
  const sourcemeta::core::URI uri{"file://"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{});
}

TEST(URI_to_path, roundtrip_unix_absolute) {
  const std::filesystem::path original{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, roundtrip_unix_with_spaces) {
  const std::filesystem::path original{"/foo/My Folder/test file.txt"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, roundtrip_unix_with_special_chars) {
  const std::filesystem::path original{"/foo/has#hash?value%"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, roundtrip_unix_trailing_slash) {
  const std::filesystem::path original{"/foo/bar/"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, roundtrip_windows_drive) {
  const std::filesystem::path original{R"(C:\Program Files\Test)"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(result, original);
#else
  EXPECT_EQ(result, std::filesystem::path{"/C:/Program Files/Test"});
#endif
}

TEST(URI_to_path, roundtrip_windows_unc) {
  const std::filesystem::path original{R"(\\server\share\file.txt)"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(result, original);
#else
  EXPECT_EQ(result, std::filesystem::path{"//server/share/file.txt"});
#endif
}

TEST(URI_to_path, roundtrip_unicode) {
  const std::filesystem::path original{u8"/data/éclair.txt"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}
