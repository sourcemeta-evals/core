#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, non_file_uri_returns_path_component) {
  const sourcemeta::core::URI uri{"https://example.com/foo%20bar/baz?x=1#frag"};
  EXPECT_EQ(uri.to_path().string(), "/foo%20bar/baz");
}

TEST(URI_to_path, file_unix_path) {
  const sourcemeta::core::URI uri{"file:///tmp/My%20Folder/test.txt"};
  EXPECT_EQ(uri.to_path().string(), "/tmp/My Folder/test.txt");
}

TEST(URI_to_path, file_windows_drive_path) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test.txt"};
  EXPECT_EQ(uri.to_path().string(), R"(C:\Program Files\Test.txt)");
}

TEST(URI_to_path, file_windows_unc_path) {
  const sourcemeta::core::URI uri{"file://server/My%20Share/a%20b.txt"};
  EXPECT_EQ(uri.to_path().string(), R"(\\server\My Share\a b.txt)");
}

TEST(URI_to_path, file_localhost_unix_path) {
  const sourcemeta::core::URI uri{"file://localhost/etc/hosts"};
  EXPECT_EQ(uri.to_path().string(), "/etc/hosts");
}
