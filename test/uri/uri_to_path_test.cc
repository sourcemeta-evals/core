#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar/baz"});
}

TEST(URI_to_path, unix_with_space_and_reserved) {
  const sourcemeta::core::URI uri{
      "file:///foo/My%20Folder/has%23hash%3Fvalue%25"};
  EXPECT_EQ(uri.to_path(),
            std::filesystem::path{"/foo/My Folder/has#hash?value%"});
}

TEST(URI_to_path, unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar/"});
}

TEST(URI_to_path, windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  const auto result = uri.to_path();
  // On UNIX, this will be /C:/Program Files/Test (no drive letter handling)
  // On Windows, this will be C:\Program Files\Test
#ifdef _WIN32
  EXPECT_EQ(result, std::filesystem::path{"C:\\Program Files\\Test"});
#else
  EXPECT_EQ(result, std::filesystem::path{"C:/Program Files/Test"});
#endif
}

TEST(URI_to_path, windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const auto result = uri.to_path();
#ifdef _WIN32
  EXPECT_EQ(result, std::filesystem::path{"c:\\temp\\logs"});
#else
  EXPECT_EQ(result, std::filesystem::path{"c:/temp/logs"});
#endif
}

TEST(URI_to_path, windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const auto result = uri.to_path();
#ifdef _WIN32
  EXPECT_EQ(result, std::filesystem::path{"D:\\"});
#else
  EXPECT_EQ(result, std::filesystem::path{"D:/"});
#endif
}

TEST(URI_to_path, windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  const auto result = uri.to_path();
#ifdef _WIN32
  EXPECT_EQ(result, std::filesystem::path{"C:\\foo\\bar\\"});
#else
  EXPECT_EQ(result, std::filesystem::path{"C:/foo/bar/"});
#endif
}

TEST(URI_to_path, windows_percent_and_plus) {
  const sourcemeta::core::URI uri{"file:///C:/path/50%25+plus.txt"};
  const auto result = uri.to_path();
#ifdef _WIN32
  EXPECT_EQ(result, std::filesystem::path{"C:\\path\\50%+plus.txt"});
#else
  EXPECT_EQ(result, std::filesystem::path{"C:/path/50%+plus.txt"});
#endif
}

TEST(URI_to_path, windows_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto result = uri.to_path();
#ifdef _WIN32
  EXPECT_EQ(result, std::filesystem::path{"\\\\server\\share\\file.txt"});
#else
  EXPECT_EQ(result, std::filesystem::path{"\\\\server/share/file.txt"});
#endif
}

TEST(URI_to_path, windows_unc_with_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  const auto result = uri.to_path();
#ifdef _WIN32
  EXPECT_EQ(result, std::filesystem::path{"\\\\srv\\My Docs\\a b.txt"});
#else
  EXPECT_EQ(result, std::filesystem::path{"\\\\srv/My Docs/a b.txt"});
#endif
}

TEST(URI_to_path, unicode_unix) {
  // U+00E9 (é) is percent-encoded as %C3%A9 in UTF-8
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{u8"/data/éclair.txt"});
}

TEST(URI_to_path, unicode_windows) {
  // U+00E9 (é) is percent-encoded as %C3%A9 in UTF-8
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  const auto result = uri.to_path();
#ifdef _WIN32
  EXPECT_EQ(result, std::filesystem::path{u8"C:\\data\\résumé.doc"});
#else
  EXPECT_EQ(result, std::filesystem::path{u8"C:/data/résumé.doc"});
#endif
}

TEST(URI_to_path, non_file_uri_http) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, non_file_uri_with_encoded) {
  const sourcemeta::core::URI uri{"https://example.com/foo%20bar/baz"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo bar/baz"});
}

TEST(URI_to_path, non_file_uri_no_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{});
}

TEST(URI_to_path, relative_uri_path) {
  const sourcemeta::core::URI uri{"foo/bar/baz"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"foo/bar/baz"});
}

TEST(URI_to_path, file_uri_root) {
  const sourcemeta::core::URI uri{"file:///"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/"});
}
