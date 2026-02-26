#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

#include <filesystem> // std::filesystem::path

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

TEST(URI_to_path, file_unix_with_encoded_special) {
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

TEST(URI_to_path, file_unc_with_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"//srv/My Docs/a b.txt"});
}

TEST(URI_to_path, file_unicode) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{u8"/data/\u00E9clair.txt"});
}

TEST(URI_to_path, https_uri_returns_path) {
  const sourcemeta::core::URI uri{"https://www.example.com/foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, http_uri_returns_path) {
  const sourcemeta::core::URI uri{"http://localhost:8080/api/v1/resource"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/api/v1/resource"});
}

TEST(URI_to_path, no_path_returns_empty) {
  const sourcemeta::core::URI uri{"mailto:joe@example.com"};
  // mailto URIs have an opaque path, but let's check what happens
  const auto result = uri.to_path();
  // The path component of a mailto is the email address
  EXPECT_FALSE(result.empty());
}

TEST(URI_to_path, fragment_only_returns_empty) {
  const sourcemeta::core::URI uri{"#foo"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{});
}

TEST(URI_to_path, empty_uri_returns_empty) {
  const sourcemeta::core::URI uri{""};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{});
}

TEST(URI_to_path, file_roundtrip_unix) {
  const std::filesystem::path original{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  EXPECT_EQ(uri.to_path(), original);
}

TEST(URI_to_path, file_roundtrip_unix_with_space) {
  const std::filesystem::path original{"/foo/My Folder/has#hash?value%"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  EXPECT_EQ(uri.to_path(), original);
}

TEST(URI_to_path, file_roundtrip_windows_drive) {
  const std::filesystem::path original{"C:/Program Files/Test"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  EXPECT_EQ(uri.to_path(), original);
}

TEST(URI_to_path, relative_path_uri) {
  const sourcemeta::core::URI uri{"foo/bar/baz"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"foo/bar/baz"});
}
