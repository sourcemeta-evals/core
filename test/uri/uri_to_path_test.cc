#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

#include <filesystem>

TEST(URI_to_path, file_unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar/baz"});
}

TEST(URI_to_path, file_unix_root) {
  const sourcemeta::core::URI uri{"file:///"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/"});
}

TEST(URI_to_path, file_unix_with_encoded_space) {
  const sourcemeta::core::URI uri{"file:///foo/My%20Folder/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/My Folder/bar"});
}

TEST(URI_to_path, file_unix_with_encoded_hash) {
  const sourcemeta::core::URI uri{"file:///foo/has%23hash"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/has#hash"});
}

TEST(URI_to_path, file_unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar/"});
}

TEST(URI_to_path, file_windows_drive) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"C:/Program Files/Test"});
}

TEST(URI_to_path, file_windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"c:/temp/logs"});
}

TEST(URI_to_path, file_windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"D:/"});
}

TEST(URI_to_path, file_windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"C:/foo/bar/"});
}

TEST(URI_to_path, file_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"//server/share/file.txt"});
}

TEST(URI_to_path, file_unc_with_encoded_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"//srv/My Docs/a b.txt"});
}

TEST(URI_to_path, file_unicode_encoded) {
  // %C3%A9 is UTF-8 encoding of U+00E9 (é)
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{u8"/data/éclair.txt"});
}

TEST(URI_to_path, file_percent_encoded) {
  const sourcemeta::core::URI uri{"file:///C:/path/50%25+plus.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"C:/path/50%+plus.txt"});
}

TEST(URI_to_path, non_file_http) {
  const sourcemeta::core::URI uri{"https://www.sourcemeta.com/foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, non_file_http_no_path) {
  const sourcemeta::core::URI uri{"https://www.sourcemeta.com"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{});
}

TEST(URI_to_path, non_file_with_query_and_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/path?q=1#frag"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/path"});
}

TEST(URI_to_path, relative_path_only) {
  const sourcemeta::core::URI uri{"foo/bar/baz"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"foo/bar/baz"});
}

TEST(URI_to_path, absolute_path_only) {
  const sourcemeta::core::URI uri{"/foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, file_empty_path) {
  const sourcemeta::core::URI uri{"file://"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{});
}

TEST(URI_to_path, roundtrip_unix) {
  const std::filesystem::path original{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  EXPECT_EQ(uri.to_path(), original);
}

TEST(URI_to_path, roundtrip_unix_with_space) {
  const std::filesystem::path original{"/foo/My Folder/bar"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  EXPECT_EQ(uri.to_path(), original);
}
