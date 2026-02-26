#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, non_file_uri_returns_path_component) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, non_file_uri_without_path_returns_empty_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{});
}

TEST(URI_to_path, file_unix_path) {
  const sourcemeta::core::URI uri{"file:///foo/My%20Folder/has%23hash"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/My Folder/has#hash"});
}

TEST(URI_to_path, file_windows_drive_path) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  EXPECT_EQ(uri.to_path().string(), R"(C:\Program Files\Test)");
}

TEST(URI_to_path, file_windows_unc_path) {
  const sourcemeta::core::URI uri{"file://server/share/My%20Docs/a%20b.txt"};
  EXPECT_EQ(uri.to_path().string(), R"(\\server\share\My Docs\a b.txt)");
}

TEST(URI_to_path, file_localhost_unix_path) {
  const sourcemeta::core::URI uri{"file://localhost/etc/hosts"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/etc/hosts"});
}
