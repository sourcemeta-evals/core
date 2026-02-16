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

TEST(URI_to_path, file_unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar/"});
}

TEST(URI_to_path, file_windows_drive) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
#if defined(_WIN32)
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"C:\\Program Files\\Test"});
#else
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/C:/Program Files/Test"});
#endif
}

TEST(URI_to_path, file_windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
#if defined(_WIN32)
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"c:\\temp\\logs"});
#else
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/c:/temp/logs"});
#endif
}

TEST(URI_to_path, file_windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
#if defined(_WIN32)
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"D:\\"});
#else
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/D:/"});
#endif
}

TEST(URI_to_path, file_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
#if defined(_WIN32)
  EXPECT_EQ(uri.to_path(),
            std::filesystem::path{"\\\\server\\share\\file.txt"});
#else
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"//server/share/file.txt"});
#endif
}

TEST(URI_to_path, file_unc_with_encoded_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
#if defined(_WIN32)
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"\\\\srv\\My Docs\\a b.txt"});
#else
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"//srv/My Docs/a b.txt"});
#endif
}

TEST(URI_to_path, file_unicode_encoded) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  EXPECT_EQ(uri.to_path(),
            std::filesystem::path{std::string{"/data/\xC3\xA9"} + "clair.txt"});
}

TEST(URI_to_path, non_file_http) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, non_file_http_no_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{});
}

TEST(URI_to_path, non_file_with_query_and_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/foo?q=1#bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo"});
}

TEST(URI_to_path, file_empty_path) {
  const sourcemeta::core::URI uri{"file://"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{});
}

TEST(URI_to_path, file_percent_encoded_path_segment) {
  const sourcemeta::core::URI uri{"file:///path/50%25+plus.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/path/50%+plus.txt"});
}
