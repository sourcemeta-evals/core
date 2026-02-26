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

TEST(URI_to_path, file_unix_with_encoded_special_chars) {
  const sourcemeta::core::URI uri{
      "file:///foo/My%20Folder/has%23hash%3Fvalue%25"};
  EXPECT_EQ(uri.to_path(),
            std::filesystem::path{"/foo/My Folder/has#hash?value%"});
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

TEST(URI_to_path, file_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"//server/share/file.txt"});
}

TEST(URI_to_path, file_unc_with_encoded_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"//srv/My Docs/a b.txt"});
}

TEST(URI_to_path, file_unicode_encoded) {
  // %C3%A9 is the UTF-8 encoding of U+00E9 (é)
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{u8"/data/\u00E9clair.txt"});
}

TEST(URI_to_path, https_uri_returns_path) {
  const sourcemeta::core::URI uri{"https://www.example.com/foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, http_uri_with_encoded_path) {
  const sourcemeta::core::URI uri{"http://example.com/a%20b/c%20d"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/a b/c d"});
}

TEST(URI_to_path, relative_uri_no_scheme) {
  const sourcemeta::core::URI uri{""};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{});
}

TEST(URI_to_path, file_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar/"});
}

TEST(URI_to_path, file_windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"C:/foo/bar/"});
}

TEST(URI_to_path, roundtrip_unix) {
  const std::filesystem::path original{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  EXPECT_EQ(uri.to_path(), original);
}

TEST(URI_to_path, roundtrip_unix_with_space) {
  const std::filesystem::path original{"/foo/My Folder/has#hash?value%"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  EXPECT_EQ(uri.to_path(), original);
}
