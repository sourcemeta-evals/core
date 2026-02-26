#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, file_unix) {
  const sourcemeta::core::URI uri{"file:///tmp/My%20Folder/file.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/tmp/My Folder/file.txt"});
}

TEST(URI_to_path, file_windows_drive) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  EXPECT_EQ(uri.to_path().string(), R"(C:\Program Files\Test)");
}

TEST(URI_to_path, file_windows_unc) {
  const sourcemeta::core::URI uri{"file://server/My%20Docs/a%20b.txt"};
  EXPECT_EQ(uri.to_path().string(), R"(\\server\My Docs\a b.txt)");
}

TEST(URI_to_path, non_file_uses_uri_path_component) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}
