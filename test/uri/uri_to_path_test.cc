#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

#include <filesystem>

TEST(URI_to_path, unix_absolute_path) {
  const sourcemeta::core::URI uri{"file:///home/user/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/home/user/file.txt");
}

TEST(URI_to_path, unix_root) {
  const sourcemeta::core::URI uri{"file:///"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/");
}

TEST(URI_to_path, localhost_unix_path) {
  const sourcemeta::core::URI uri{"file://localhost/etc/hosts"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/etc/hosts");
}

TEST(URI_to_path, windows_absolute_path_triple_slash) {
  const sourcemeta::core::URI uri{"file:///C:/Windows/System32"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "C:/Windows/System32");
}

TEST(URI_to_path, windows_lowercase_drive) {
  const sourcemeta::core::URI uri{"file:///c:/temp/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "c:/temp/file.txt");
}

TEST(URI_to_path, unc_path) {
  const sourcemeta::core::URI uri{"file://server/share/dir/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "//server/share/dir/file.txt");
}

TEST(URI_to_path, unc_path_minimal) {
  const sourcemeta::core::URI uri{"file://server/share"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "//server/share");
}

TEST(URI_to_path, percent_encoded_space) {
  const sourcemeta::core::URI uri{"file:///home/a%20b/c"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/home/a b/c");
}

TEST(URI_to_path, percent_encoded_brackets) {
  const sourcemeta::core::URI uri{"file:///home/c%5B1%5D"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/home/c[1]");
}

TEST(URI_to_path, percent_encoded_mixed) {
  const sourcemeta::core::URI uri{"file:///home/a%20b/c%5B1%5D"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/home/a b/c[1]");
}

TEST(URI_to_path, non_file_uri_http) {
  const sourcemeta::core::URI uri{"https://example.com/a/b%20c"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/a/b%20c");
}

TEST(URI_to_path, non_file_uri_ftp) {
  const sourcemeta::core::URI uri{"ftp://ftp.example.com/pub/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/pub/file.txt");
}

TEST(URI_to_path, non_file_uri_no_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "");
}

TEST(URI_to_path, file_uri_empty_path) {
  const sourcemeta::core::URI uri{"file://"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "");
}

TEST(URI_to_path, complex_unix_path) {
  const sourcemeta::core::URI uri{"file:///usr/local/bin/my-app"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/usr/local/bin/my-app");
}

TEST(URI_to_path, windows_path_with_spaces) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/App/file.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "C:/Program Files/App/file.txt");
}

TEST(URI_to_path, localhost_case_sensitive) {
  const sourcemeta::core::URI uri{"file://localhost/home/user"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/home/user");
}

TEST(URI_to_path, windows_drive_uppercase) {
  const sourcemeta::core::URI uri{"file:///D:/Data/file.dat"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "D:/Data/file.dat");
}

TEST(URI_to_path, windows_drive_z) {
  const sourcemeta::core::URI uri{"file:///Z:/backup"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "Z:/backup");
}

TEST(URI_to_path, unc_with_subdirectories) {
  const sourcemeta::core::URI uri{"file://nas/storage/documents/report.pdf"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "//nas/storage/documents/report.pdf");
}

TEST(URI_to_path, percent_encoded_unicode) {
  const sourcemeta::core::URI uri{"file:///home/user/%E2%9C%93"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/home/user/✓");
}

TEST(URI_to_path, relative_uri_with_path) {
  const sourcemeta::core::URI uri{"../relative/path"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "../relative/path");
}

TEST(URI_to_path, absolute_path_no_scheme) {
  const sourcemeta::core::URI uri{"/absolute/path"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.generic_string(), "/absolute/path");
}
