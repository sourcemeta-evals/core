#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_to_path, unix_absolute) {
  const sourcemeta::core::URI uri{"file:///foo/bar/baz"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/bar/baz");
}

TEST(URI_to_path, unix_with_space_and_reserved) {
  const sourcemeta::core::URI uri{
      "file:///foo/My%20Folder/has%23hash%3Fvalue%25"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/My Folder/has#hash?value%");
}

TEST(URI_to_path, unix_trailing_slash) {
  const sourcemeta::core::URI uri{"file:///foo/bar/"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/bar/");
}

TEST(URI_to_path, unix_root) {
  const sourcemeta::core::URI uri{"file:///"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/");
}

TEST(URI_to_path, unicode_unix) {
  const sourcemeta::core::URI uri{"file:///data/%C3%A9clair.txt"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/data/éclair.txt");
}

TEST(URI_to_path, windows_drive_absolute) {
  const sourcemeta::core::URI uri{"file:///C:/Program%20Files/Test"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path.string(), "C:\\Program Files\\Test");
#else
  EXPECT_TRUE(path.string().find("C:") != std::string::npos);
#endif
}

TEST(URI_to_path, windows_drive_lowercase) {
  const sourcemeta::core::URI uri{"file:///c:/temp/logs"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path.string(), "c:\\temp\\logs");
#else
  EXPECT_TRUE(path.string().find("c:") != std::string::npos);
#endif
}

TEST(URI_to_path, windows_drive_root) {
  const sourcemeta::core::URI uri{"file:///D:/"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path.string(), "D:\\");
#else
  EXPECT_TRUE(path.string().find("D:") != std::string::npos);
#endif
}

TEST(URI_to_path, windows_percent_and_plus) {
  const sourcemeta::core::URI uri{"file:///C:/path/50%25+plus.txt"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path.string(), "C:\\path\\50%+plus.txt");
#else
  EXPECT_TRUE(path.string().find("C:") != std::string::npos);
  EXPECT_TRUE(path.string().find("50%+plus") != std::string::npos);
#endif
}

TEST(URI_to_path, unicode_windows) {
  const sourcemeta::core::URI uri{"file:///C:/data/r%C3%A9sum%C3%A9.doc"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path.string(), "C:\\data\\résumé.doc");
#else
  EXPECT_TRUE(path.string().find("résumé") != std::string::npos);
#endif
}

TEST(URI_to_path, windows_unc_simple) {
  const sourcemeta::core::URI uri{"file://server/share/file.txt"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path.string(), "\\\\server\\share\\file.txt");
#else
  EXPECT_TRUE(path.string().find("server") != std::string::npos);
#endif
}

TEST(URI_to_path, windows_unc_with_space) {
  const sourcemeta::core::URI uri{"file://srv/My%20Docs/a%20b.txt"};
  const auto path{uri.to_path()};
#ifdef _WIN32
  EXPECT_EQ(path.string(), "\\\\srv\\My Docs\\a b.txt");
#else
  EXPECT_TRUE(path.string().find("My Docs") != std::string::npos);
#endif
}

TEST(URI_to_path, https_returns_path_component) {
  const sourcemeta::core::URI uri{"https://www.example.com/foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/foo/bar");
}

TEST(URI_to_path, http_with_encoded_chars) {
  const sourcemeta::core::URI uri{"http://example.com/path%20with%20spaces"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "/path%20with%20spaces");
}

TEST(URI_to_path, urn_returns_path) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "example:schema");
}

TEST(URI_to_path, relative_uri_returns_path) {
  const sourcemeta::core::URI uri{"../foo/bar"};
  const auto path{uri.to_path()};
  EXPECT_EQ(path.string(), "../foo/bar");
}

TEST(URI_to_path, empty_uri) {
  const sourcemeta::core::URI uri{""};
  const auto path{uri.to_path()};
  EXPECT_TRUE(path.empty());
}
