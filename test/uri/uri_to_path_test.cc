#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

// Non-file URI tests
TEST(URI_to_path, http_simple_path) {
  const sourcemeta::core::URI uri{"http://example.com/foo/bar"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "/foo/bar");
}

TEST(URI_to_path, http_with_query_and_fragment) {
  const sourcemeta::core::URI uri{"http://example.com/foo/bar?x=1#frag"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "/foo/bar");
}

TEST(URI_to_path, http_with_percent_encoding) {
  const sourcemeta::core::URI uri{"http://example.com/space%20here"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "/space here");
}

TEST(URI_to_path, https_root_path) {
  const sourcemeta::core::URI uri{"https://example.com/"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "/");
}

TEST(URI_to_path, urn_empty_path) {
  const sourcemeta::core::URI uri{"urn:example:foo"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "example:foo");
}

// file:// URI tests - UNIX paths
TEST(URI_to_path, file_unix_absolute) {
  const sourcemeta::core::URI uri{"file:///etc/hosts"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "/etc/hosts");
}

TEST(URI_to_path, file_unix_with_localhost) {
  const sourcemeta::core::URI uri{"file://localhost/usr/bin/env"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "/usr/bin/env");
}

TEST(URI_to_path, file_unix_root) {
  const sourcemeta::core::URI uri{"file:///"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "/");
}

TEST(URI_to_path, file_unix_with_spaces) {
  const sourcemeta::core::URI uri{"file:///path/with%20spaces/file.txt"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "/path/with spaces/file.txt");
}

// file:// URI tests - Windows paths
TEST(URI_to_path, file_windows_drive_c) {
  const sourcemeta::core::URI uri{"file:///C:/Windows/System32"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "C:/Windows/System32");
}

TEST(URI_to_path, file_windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///d:/dir/file.txt"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "d:/dir/file.txt");
}

TEST(URI_to_path, file_windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "D:/");
}

TEST(URI_to_path, file_windows_with_percent_encoding) {
  const sourcemeta::core::URI uri{"file:///C:/path%20with%20spaces/file.txt"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "C:/path with spaces/file.txt");
}

TEST(URI_to_path, file_windows_with_special_chars) {
  const sourcemeta::core::URI uri{"file:///C:/path/file%23name.txt"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "C:/path/file#name.txt");
}

// file:// URI tests - UNC paths
// Note: std::filesystem::path normalizes // to / on POSIX systems
TEST(URI_to_path, file_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/folder/file.txt"};
  const auto path = uri.to_path();
  // On POSIX, //server becomes /server (path normalization)
  EXPECT_EQ(path.generic_string(), "/server/share/folder/file.txt");
}

TEST(URI_to_path, file_unc_with_spaces) {
  const sourcemeta::core::URI uri{"file://server/Shared%20Docs/Readme.txt"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "/server/Shared Docs/Readme.txt");
}

TEST(URI_to_path, file_unc_root_share) {
  const sourcemeta::core::URI uri{"file://server/share"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "/server/share");
}

TEST(URI_to_path, file_unc_with_trailing_slash) {
  const sourcemeta::core::URI uri{"file://server/share/"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "/server/share/");
}

// Edge cases
TEST(URI_to_path, file_localhost_empty_path) {
  const sourcemeta::core::URI uri{"file://localhost"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "");
}

TEST(URI_to_path, file_empty_host_empty_path) {
  const sourcemeta::core::URI uri{"file://"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "");
}

TEST(URI_to_path, http_no_path) {
  const sourcemeta::core::URI uri{"http://example.com"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "");
}

TEST(URI_to_path, relative_uri) {
  const sourcemeta::core::URI uri{"foo/bar"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "foo/bar");
}

TEST(URI_to_path, fragment_only) {
  const sourcemeta::core::URI uri{"#fragment"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "");
}

TEST(URI_to_path, file_with_encoded_slash) {
  const sourcemeta::core::URI uri{"file:///path/with%2Fencoded%2Fslash"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "/path/with/encoded/slash");
}

TEST(URI_to_path, file_with_plus_sign) {
  const sourcemeta::core::URI uri{"file:///path/file+name.txt"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "/path/file+name.txt");
}

TEST(URI_to_path, file_with_percent_in_name) {
  const sourcemeta::core::URI uri{"file:///path/50%25discount.txt"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.generic_string(), "/path/50%discount.txt");
}
