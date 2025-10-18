#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, unix_absolute) {
  const sourcemeta::core::URI uri{"file:///etc/hosts"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/etc/hosts"});
}

TEST(URI_to_path, unix_localhost) {
  const sourcemeta::core::URI uri{"file://localhost/etc/hosts"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/etc/hosts"});
}

TEST(URI_to_path, unix_with_percent_encoding) {
  const sourcemeta::core::URI uri{"file:///tmp/Hello%20World.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/tmp/Hello World.txt"});
}

TEST(URI_to_path, unix_with_special_chars) {
  const sourcemeta::core::URI uri{
      "file:///foo/My%20Folder/has%23hash%3Fvalue%25"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/My Folder/has#hash?value%"});
}

TEST(URI_to_path, unix_unc_path) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path, std::filesystem::path{R"(\\server\share\file.txt)"});
#else
  EXPECT_EQ(path, std::filesystem::path{"//server/share/file.txt"});
#endif
}

TEST(URI_to_path, unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo/bar/"});
}

TEST(URI_to_path, windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path, std::filesystem::path{R"(C:\Program Files\Test)"});
#else
  EXPECT_EQ(path, std::filesystem::path{"/C:/Program Files/Test"});
#endif
}

TEST(URI_to_path, windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path, std::filesystem::path{R"(c:\temp\logs)"});
#else
  EXPECT_EQ(path, std::filesystem::path{"/c:/temp/logs"});
#endif
}

TEST(URI_to_path, windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path, std::filesystem::path{R"(D:\)"});
#else
  EXPECT_EQ(path, std::filesystem::path{"/D:/"});
#endif
}

TEST(URI_to_path, windows_localhost_drive) {
  const sourcemeta::core::URI uri{"file://localhost/C:/Program%20Files/App"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path, std::filesystem::path{R"(C:\Program Files\App)"});
#else
  EXPECT_EQ(path, std::filesystem::path{"/C:/Program Files/App"});
#endif
}

TEST(URI_to_path, windows_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/folder/file"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path, std::filesystem::path{R"(\\server\share\folder\file)"});
#else
  EXPECT_EQ(path, std::filesystem::path{"//server/share/folder/file"});
#endif
}

TEST(URI_to_path, windows_unc_with_spaces) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path, std::filesystem::path{R"(\\srv\My Docs\a b.txt)"});
#else
  EXPECT_EQ(path, std::filesystem::path{"//srv/My Docs/a b.txt"});
#endif
}

TEST(URI_to_path, http_uri) {
  const sourcemeta::core::URI uri{"http://example.com/foo%20bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/foo bar"});
}

TEST(URI_to_path, https_uri) {
  const sourcemeta::core::URI uri{"https://example.com/a/b"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/a/b"});
}

TEST(URI_to_path, non_file_uri_with_encoding) {
  const sourcemeta::core::URI uri{"https://example.com/path%2Fwith%2Fslashes"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"/path/with/slashes"});
}

TEST(URI_to_path, relative_uri) {
  const sourcemeta::core::URI uri{"foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{"foo/bar"});
}

TEST(URI_to_path, empty_path) {
  const sourcemeta::core::URI uri{"http://example.com"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{""});
}

TEST(URI_to_path, unicode_unix) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path, std::filesystem::path{u8"/data/éclair.txt"});
}

TEST(URI_to_path, unicode_windows) {
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path, std::filesystem::path{u8R"(C:\data\résumé.doc)"});
#else
  EXPECT_EQ(path, std::filesystem::path{u8"/C:/data/résumé.doc"});
#endif
}

TEST(URI_to_path, roundtrip_unix) {
  const std::filesystem::path original{"/foo/bar/baz"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, roundtrip_unix_with_spaces) {
  const std::filesystem::path original{"/tmp/Hello World.txt"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

#ifdef _WIN32
TEST(URI_to_path, roundtrip_windows_drive) {
  const std::filesystem::path original{R"(C:\Program Files\Test)"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}

TEST(URI_to_path, roundtrip_windows_unc) {
  const std::filesystem::path original{R"(\\server\share\file.txt)"};
  const auto uri{sourcemeta::core::URI::from_path(original)};
  const auto result{uri.to_path()};
  EXPECT_EQ(result, original);
}
#endif
