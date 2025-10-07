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
#ifdef _WIN32
  EXPECT_EQ(path.string(), "C:\\Program Files\\Test");
#else
  EXPECT_EQ(path.generic_string(), "C:/Program Files/Test");
#endif
}

TEST(URI_to_path, windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path.string(), "c:\\temp\\logs");
#else
  EXPECT_EQ(path.generic_string(), "c:/temp/logs");
#endif
}

TEST(URI_to_path, windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path.string(), "D:\\");
#else
  EXPECT_EQ(path.generic_string(), "D:/");
#endif
}

TEST(URI_to_path, windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path.string(), "C:\\foo\\bar\\");
#else
  EXPECT_EQ(path.generic_string(), "C:/foo/bar/");
#endif
}

TEST(URI_to_path, windows_percent_and_plus) {
  const sourcemeta::core::URI uri{"file:///C:/path/50%25+plus.txt"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path.string(), "C:\\path\\50%+plus.txt");
#else
  EXPECT_EQ(path.generic_string(), "C:/path/50%+plus.txt");
#endif
}

TEST(URI_to_path, windows_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path.string(), "\\\\server\\share\\file.txt");
#else
  EXPECT_EQ(path.generic_string(), "\\\\server\\share\\file.txt");
#endif
}

TEST(URI_to_path, windows_unc_with_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path.string(), "\\\\srv\\My Docs\\a b.txt");
#else
  EXPECT_EQ(path.generic_string(), "\\\\srv\\My Docs\\a b.txt");
#endif
}

TEST(URI_to_path, unicode_unix) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/data/éclair.txt");
}

TEST(URI_to_path, unicode_windows) {
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path.string(), "C:\\data\\résumé.doc");
#else
  EXPECT_EQ(path.generic_string(), "C:/data/résumé.doc");
#endif
}

TEST(URI_to_path, round_trip_unix) {
  const std::filesystem::path original{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result.generic_string(), original.generic_string());
}

TEST(URI_to_path, round_trip_unix_with_special) {
  const std::filesystem::path original{"/foo/My Folder/has#hash?value%"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result.generic_string(), original.generic_string());
}

TEST(URI_to_path, non_file_uri_https) {
  const sourcemeta::core::URI uri{"https://www.example.com/foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/foo/bar");
}

TEST(URI_to_path, non_file_uri_http) {
  const sourcemeta::core::URI uri{"http://localhost:8000/api/v1/test"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/api/v1/test");
}

TEST(URI_to_path, non_file_uri_relative) {
  const sourcemeta::core::URI uri{"./foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "foo/bar");
}

TEST(URI_to_path, file_uri_without_path) {
  const sourcemeta::core::URI uri{"file://"};
  EXPECT_THROW(uri.to_path(), sourcemeta::core::URIError);
}

TEST(URI_to_path, uri_without_path_fragment_only) {
  const sourcemeta::core::URI uri{"#fragment"};
  EXPECT_THROW(uri.to_path(), sourcemeta::core::URIError);
}

TEST(URI_to_path, uri_without_path_empty) {
  const sourcemeta::core::URI uri{""};
  EXPECT_THROW(uri.to_path(), sourcemeta::core::URIError);
}
