#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

#include <filesystem>

TEST(URI_to_path, file_uri_unix_absolute_path) {
  const sourcemeta::core::URI uri{"file:///foo/bar"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.string(), "/foo/bar");
}

TEST(URI_to_path, file_uri_unix_root) {
  const sourcemeta::core::URI uri{"file:///"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.string(), "/");
}

TEST(URI_to_path, file_uri_unix_nested_path) {
  const sourcemeta::core::URI uri{"file:///home/user/documents/file.txt"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.string(), "/home/user/documents/file.txt");
}

TEST(URI_to_path, file_uri_with_percent_encoding) {
  const sourcemeta::core::URI uri{"file:///foo%20bar/baz"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.string(), "/foo bar/baz");
}

TEST(URI_to_path, file_uri_with_special_characters) {
  const sourcemeta::core::URI uri{"file:///foo%2Fbar"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.string(), "/foo/bar");
}

TEST(URI_to_path, non_file_uri_http) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.string(), "/foo/bar");
}

TEST(URI_to_path, non_file_uri_with_query) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar?query=value"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.string(), "/foo/bar");
}

TEST(URI_to_path, non_file_uri_with_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar#section"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.string(), "/foo/bar");
}

TEST(URI_to_path, uri_with_no_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  const auto path = uri.to_path();
  EXPECT_TRUE(path.empty());
}

TEST(URI_to_path, relative_uri) {
  const sourcemeta::core::URI uri{"foo/bar"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.string(), "foo/bar");
}

TEST(URI_to_path, empty_uri) {
  const sourcemeta::core::URI uri{""};
  const auto path = uri.to_path();
  EXPECT_TRUE(path.empty());
}

TEST(URI_to_path, roundtrip_unix_path) {
  const std::filesystem::path original{"/tmp/test/file.txt"};
  const auto uri = sourcemeta::core::URI::from_path(original);
  const auto result = uri.to_path();
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, roundtrip_unix_path_with_spaces) {
  const std::filesystem::path original{"/tmp/test dir/file name.txt"};
  const auto uri = sourcemeta::core::URI::from_path(original);
  const auto result = uri.to_path();
  EXPECT_EQ(result, original);
}

#ifdef _WIN32
TEST(URI_to_path, file_uri_windows_absolute_path) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.string(), "C:\\foo\\bar");
}

TEST(URI_to_path, file_uri_windows_drive_only) {
  const sourcemeta::core::URI uri{"file:///C:/"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.string(), "C:\\");
}

TEST(URI_to_path, file_uri_windows_unc_path) {
  const sourcemeta::core::URI uri{"file://host/share/path/file.txt"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.string(), "\\\\host\\share\\path\\file.txt");
}

TEST(URI_to_path, file_uri_windows_unc_path_with_encoding) {
  const sourcemeta::core::URI uri{"file://server/share%20name/file.txt"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.string(), "\\\\server\\share name\\file.txt");
}

TEST(URI_to_path, roundtrip_windows_path) {
  const std::filesystem::path original{"C:\\Users\\test\\file.txt"};
  const auto uri = sourcemeta::core::URI::from_path(original);
  const auto result = uri.to_path();
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, roundtrip_windows_unc_path) {
  const std::filesystem::path original{"\\\\server\\share\\file.txt"};
  const auto uri = sourcemeta::core::URI::from_path(original);
  const auto result = uri.to_path();
  EXPECT_EQ(result, original);
}
#else
TEST(URI_to_path, file_uri_windows_drive_on_unix) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar"};
  const auto path = uri.to_path();
  EXPECT_EQ(path.string(), "C:/foo/bar");
}
#endif
