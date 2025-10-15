#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

#include <string>

TEST(URI_to_path, file_unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{"/foo/bar/baz"});
}

TEST(URI_to_path, file_unix_with_escaped_space) {
  const sourcemeta::core::URI uri{"file:///foo/My%20Folder/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{"/foo/My Folder/file.txt"});
}

TEST(URI_to_path, file_unix_with_escaped_hash) {
  const sourcemeta::core::URI uri{"file:///foo/has%23hash"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{"/foo/has#hash"});
}

TEST(URI_to_path, file_unix_with_escaped_question) {
  const sourcemeta::core::URI uri{"file:///foo/has%3Fquestion"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{"/foo/has?question"});
}

TEST(URI_to_path, file_unix_with_escaped_percent) {
  const sourcemeta::core::URI uri{"file:///foo/has%25percent"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{"/foo/has%percent"});
}

TEST(URI_to_path, file_unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{"/foo/bar/"});
}

TEST(URI_to_path, file_unix_root) {
  const sourcemeta::core::URI uri{"file:///"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{"/"});
}

TEST(URI_to_path, file_windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  const auto path{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(path.string(), std::string{"C:\\Program Files\\Test"});
#else
  EXPECT_EQ(path.string(), std::string{"C:/Program Files/Test"});
#endif
}

TEST(URI_to_path, file_windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const auto path{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(path.string(), std::string{"c:\\temp\\logs"});
#else
  EXPECT_EQ(path.string(), std::string{"c:/temp/logs"});
#endif
}

TEST(URI_to_path, file_windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const auto path{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(path.string(), std::string{"D:\\"});
#else
  EXPECT_EQ(path.string(), std::string{"D:/"});
#endif
}

TEST(URI_to_path, file_windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  const auto path{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(path.string(), std::string{"C:\\foo\\bar\\"});
#else
  EXPECT_EQ(path.string(), std::string{"C:/foo/bar/"});
#endif
}

TEST(URI_to_path, file_windows_percent_and_plus) {
  const sourcemeta::core::URI uri{"file:///C:/path/50%25+plus.txt"};
  const auto path{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(path.string(), std::string{"C:\\path\\50%+plus.txt"});
#else
  EXPECT_EQ(path.string(), std::string{"C:/path/50%+plus.txt"});
#endif
}

TEST(URI_to_path, file_windows_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto path{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(path.string(), std::string{"\\\\server\\share\\file.txt"});
#else
  EXPECT_EQ(path.string(), std::string{"\\\\server/share/file.txt"});
#endif
}

TEST(URI_to_path, file_windows_unc_with_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  const auto path{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(path.string(), std::string{"\\\\srv\\My Docs\\a b.txt"});
#else
  EXPECT_EQ(path.string(), std::string{"\\\\srv/My Docs/a b.txt"});
#endif
}

TEST(URI_to_path, file_unicode_unix) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{"/data/éclair.txt"});
}

TEST(URI_to_path, file_unicode_windows) {
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  const auto path{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(path.string(), std::string{"C:\\data\\résumé.doc"});
#else
  EXPECT_EQ(path.string(), std::string{"C:/data/résumé.doc"});
#endif
}

TEST(URI_to_path, non_file_http_with_path) {
  const sourcemeta::core::URI uri{"https://www.example.com/foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{"/foo/bar"});
}

TEST(URI_to_path, non_file_http_with_query) {
  const sourcemeta::core::URI uri{"https://www.example.com/foo?bar=baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{"/foo"});
}

TEST(URI_to_path, non_file_http_root) {
  const sourcemeta::core::URI uri{"https://www.example.com/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{"/"});
}

TEST(URI_to_path, non_file_http_no_path) {
  const sourcemeta::core::URI uri{"https://www.example.com"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{""});
}

TEST(URI_to_path, relative_uri_with_path) {
  const sourcemeta::core::URI uri{"foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{"foo/bar/baz"});
}

TEST(URI_to_path, relative_uri_with_dotdot) {
  const sourcemeta::core::URI uri{"../parent/dir"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{"../parent/dir"});
}

TEST(URI_to_path, empty_uri) {
  const sourcemeta::core::URI uri{""};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{""});
}

TEST(URI_to_path, fragment_only) {
  const sourcemeta::core::URI uri{"#fragment"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{""});
}

TEST(URI_to_path, urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), std::string{"example:schema"});
}

TEST(URI_to_path, round_trip_unix) {
  const std::filesystem::path original{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, round_trip_unix_with_special_chars) {
  const std::filesystem::path original{"/foo/My Folder/has#hash?value%"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, round_trip_windows_drive) {
  const std::filesystem::path original{R"(C:\Program Files\Test)"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(result, original);
#else
  EXPECT_EQ(result.string(), std::string{"C:/Program Files/Test"});
#endif
}

TEST(URI_to_path, round_trip_windows_unc) {
  const std::filesystem::path original{R"(\\server\share\file.txt)"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
#if defined(_WIN32)
  EXPECT_EQ(result, original);
#else
  EXPECT_EQ(result.string(), std::string{"\\\\server/share/file.txt"});
#endif
}
