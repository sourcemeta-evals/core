#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/bar/baz"});
}

TEST(URI_to_path, unix_with_encoded_space) {
  const sourcemeta::core::URI uri{"file:///foo/My%20Folder/test"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/My Folder/test"});
}

TEST(URI_to_path, unix_with_encoded_special_chars) {
  const sourcemeta::core::URI uri{"file:///foo/has%23hash%3Fvalue%25"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/has#hash?value%"});
}

TEST(URI_to_path, unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/bar/"});
}

TEST(URI_to_path, unix_root) {
  const sourcemeta::core::URI uri{"file:///"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/"});
}

TEST(URI_to_path, windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path, std::filesystem::path{R"(C:\Program Files\Test)"});
#else
  // On Unix, the path constructor will keep forward slashes
  EXPECT_EQ(path.string(), "C:\\Program Files\\Test");
#endif
}

TEST(URI_to_path, windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path, std::filesystem::path{R"(c:\temp\logs)"});
#else
  EXPECT_EQ(path.string(), "c:\\temp\\logs");
#endif
}

TEST(URI_to_path, windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path, std::filesystem::path{R"(D:\)"});
#else
  EXPECT_EQ(path.string(), "D:\\");
#endif
}

TEST(URI_to_path, windows_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///C:/foo/bar/"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path, std::filesystem::path{R"(C:\foo\bar\)"});
#else
  EXPECT_EQ(path.string(), "C:\\foo\\bar\\");
#endif
}

TEST(URI_to_path, windows_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path, std::filesystem::path{R"(\\server\share\file.txt)"});
#else
  EXPECT_EQ(path.string(), "\\\\server\\share\\file.txt");
#endif
}

TEST(URI_to_path, windows_unc_with_encoded_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path, std::filesystem::path{R"(\\srv\My Docs\a b.txt)"});
#else
  EXPECT_EQ(path.string(), "\\\\srv\\My Docs\\a b.txt");
#endif
}

TEST(URI_to_path, unicode_unix) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/data/éclair.txt"});
}

TEST(URI_to_path, unicode_windows) {
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path, std::filesystem::path{R"(C:\data\résumé.doc)"});
#else
  EXPECT_EQ(path.string(), "C:\\data\\résumé.doc");
#endif
}

TEST(URI_to_path, non_file_uri_http) {
  const sourcemeta::core::URI uri{"https://example.com/foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/bar"});
}

TEST(URI_to_path, non_file_uri_with_query) {
  const sourcemeta::core::URI uri{
      "https://example.com/api/endpoint?param=value"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/api/endpoint"});
}

TEST(URI_to_path, non_file_uri_no_path) {
  const sourcemeta::core::URI uri{"https://example.com"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{});
}

TEST(URI_to_path, relative_uri) {
  const sourcemeta::core::URI uri{"foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"foo/bar/baz"});
}

TEST(URI_to_path, fragment_only) {
  const sourcemeta::core::URI uri{"#fragment"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{});
}

TEST(URI_to_path, empty_uri) {
  const sourcemeta::core::URI uri{""};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{});
}

TEST(URI_to_path, roundtrip_unix) {
  const std::filesystem::path original{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, roundtrip_windows) {
#ifdef _WIN32
  const std::filesystem::path original{R"(C:\Program Files\Test)"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
#else
  GTEST_SKIP() << "Windows-specific test";
#endif
}

TEST(URI_to_path, roundtrip_unc) {
#ifdef _WIN32
  const std::filesystem::path original{R"(\\server\share\file.txt)"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
#else
  GTEST_SKIP() << "Windows-specific test";
#endif
}
