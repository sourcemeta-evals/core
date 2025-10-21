#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

#include <filesystem>

TEST(URI_to_path, non_file_uri_http) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/foo/bar");
}

TEST(URI_to_path, non_file_uri_ftp) {
  const sourcemeta::core::URI uri{"ftp://example.org/a/b"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/a/b");
}

TEST(URI_to_path, non_file_uri_with_percent_encoding) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar%20baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/foo/bar%20baz");
}

TEST(URI_to_path, non_file_uri_empty_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "");
}

#ifdef _WIN32

TEST(URI_to_path, file_uri_windows_drive_letter) {
  const sourcemeta::core::URI uri{"file:///C:/Windows/System32"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "C:/Windows/System32");
}

TEST(URI_to_path, file_uri_windows_drive_letter_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/windows/system32"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "c:/windows/system32");
}

TEST(URI_to_path, file_uri_windows_localhost_drive_letter) {
  const sourcemeta::core::URI uri{"file://localhost/C:/Windows/System32"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "C:/Windows/System32");
}

TEST(URI_to_path, file_uri_windows_with_percent_encoding) {
  const sourcemeta::core::URI uri{"file:///C:/My%20Docs/report.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "C:/My Docs/report.txt");
}

TEST(URI_to_path, file_uri_windows_unc_path) {
  const sourcemeta::core::URI uri{"file://server/share/folder/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "//server/share/folder/file.txt");
}

TEST(URI_to_path, file_uri_windows_unc_path_with_percent_encoding) {
  const sourcemeta::core::URI uri{"file://server/share/My%20Folder/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "//server/share/My Folder/file.txt");
}

#else

TEST(URI_to_path, file_uri_unix_absolute_path) {
  const sourcemeta::core::URI uri{"file:///etc/hosts"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/etc/hosts");
}

TEST(URI_to_path, file_uri_unix_localhost) {
  const sourcemeta::core::URI uri{"file://localhost/etc/hosts"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/etc/hosts");
}

TEST(URI_to_path, file_uri_unix_with_percent_encoding) {
  const sourcemeta::core::URI uri{"file:///home/user/My%20Docs/report.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/home/user/My Docs/report.txt");
}

TEST(URI_to_path, file_uri_unix_network_path) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "//server/share/file.txt");
}

TEST(URI_to_path, file_uri_unix_root) {
  const sourcemeta::core::URI uri{"file:///"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/");
}

TEST(URI_to_path, file_uri_unix_localhost_root) {
  const sourcemeta::core::URI uri{"file://localhost/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/");
}

TEST(URI_to_path, file_uri_unix_complex_percent_encoding) {
  const sourcemeta::core::URI uri{
      "file:///home/user/file%20with%20spaces%2Band%2Bplus.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/home/user/file with spaces+and+plus.txt");
}

#endif

TEST(URI_to_path, file_uri_localhost_case_insensitive) {
  const sourcemeta::core::URI uri1{"file://localhost/test"};
  const sourcemeta::core::URI uri2{"file://LOCALHOST/test"};
  const sourcemeta::core::URI uri3{"file://Localhost/test"};
  const auto path1{uri1.to_path()};
  const auto path2{uri2.to_path()};
  const auto path3{uri3.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path1.generic_string(), "/test");
  EXPECT_EQ(path2.generic_string(), "/test");
  EXPECT_EQ(path3.generic_string(), "/test");
#else
  EXPECT_EQ(path1.generic_string(), "/test");
  EXPECT_EQ(path2.generic_string(), "/test");
  EXPECT_EQ(path3.generic_string(), "/test");
#endif
}
