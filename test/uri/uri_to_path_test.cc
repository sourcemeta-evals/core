#include <gtest/gtest.h>
#include <sourcemeta/core/uri.h>

#include <filesystem>

using sourcemeta::core::URI;

TEST(URIToPath, FileSchemeUnix) {
  const URI uri{"file:///foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}

TEST(URIToPath, FileSchemeWindows) {
  const URI uri{"file:///C:/Windows/System32"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"C:/Windows/System32"});
}

TEST(URIToPath, FileSchemeWindowsLowerCase) {
  const URI uri{"file:///c:/windows/system32"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"c:/windows/system32"});
}

TEST(URIToPath, FileSchemeUNC) {
  const URI uri{"file://server/share/file"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"//server/share/file"});
}

TEST(URIToPath, FileSchemeLocalhost) {
  const URI uri{"file://localhost/etc/hosts"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/etc/hosts"});
}

TEST(URIToPath, FileSchemeEncoded) {
  const URI uri{"file:///foo%20bar/baz"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo bar/baz"});
}

TEST(URIToPath, NonFileScheme) {
  const URI uri{"http://example.com/foo/bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}

TEST(URIToPath, NonFileSchemeEncoded) {
  const URI uri{"http://example.com/foo%20bar"};
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo%20bar"});
}

TEST(URIToPath, NoScheme) {
  const URI uri{"/foo/bar"};
  // If no scheme, it is treated as file path reconstruction?
  // My implementation checks if scheme has value and is not file.
  // If no scheme, it falls through to file logic.
  // /foo/bar -> path() returns /foo/bar.
  // Result /foo/bar.
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"/foo/bar"});
}

TEST(URIToPath, RelativePath) {
  const URI uri{"foo/bar"};
  // path() returns foo/bar for relative paths (no leading slash added if not
  // present in data)
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"foo/bar"});
}

TEST(URIToPath, FileSchemeEncodedWindows) {
  const URI uri{"file:///C%3A/Windows"};
  // %3A is :
  // path() returns /C:/Windows
  // to_path() should strip leading slash
  EXPECT_EQ(uri.to_path(), std::filesystem::path{"C:/Windows"});
}
