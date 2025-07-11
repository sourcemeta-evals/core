#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_empty, empty_string) {
  const sourcemeta::core::URI uri{""};
  EXPECT_TRUE(uri.empty());
  EXPECT_EQ(uri.recompose(), "");
}

TEST(URI_empty, fragment_only) {
  const sourcemeta::core::URI uri{"#"};
  EXPECT_FALSE(uri.empty());
  EXPECT_EQ(uri.recompose(), "#");
}

TEST(URI_empty, fragment_with_content) {
  const sourcemeta::core::URI uri{"#foo"};
  EXPECT_FALSE(uri.empty());
  EXPECT_EQ(uri.recompose(), "#foo");
}

TEST(URI_empty, relative_path) {
  const sourcemeta::core::URI uri{"foo"};
  EXPECT_FALSE(uri.empty());
  EXPECT_EQ(uri.recompose(), "foo");
}

TEST(URI_empty, absolute_path) {
  const sourcemeta::core::URI uri{"/"};
  EXPECT_FALSE(uri.empty());
  EXPECT_EQ(uri.recompose(), "/");
}

TEST(URI_empty, query_only) {
  const sourcemeta::core::URI uri{"?"};
  EXPECT_FALSE(uri.empty());
  EXPECT_EQ(uri.recompose(), "?");
}

TEST(URI_empty, query_with_content) {
  const sourcemeta::core::URI uri{"?foo=bar"};
  EXPECT_FALSE(uri.empty());
  EXPECT_EQ(uri.recompose(), "?foo=bar");
}

TEST(URI_empty, absolute_uri) {
  const sourcemeta::core::URI uri{"https://example.com"};
  EXPECT_FALSE(uri.empty());
  EXPECT_EQ(uri.recompose(), "https://example.com");
}

TEST(URI_empty, urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_FALSE(uri.empty());
  EXPECT_EQ(uri.recompose(), "urn:example:schema");
}

TEST(URI_empty, relative_with_fragment) {
  const sourcemeta::core::URI uri{"foo#bar"};
  EXPECT_FALSE(uri.empty());
  EXPECT_EQ(uri.recompose(), "foo#bar");
}

TEST(URI_empty, absolute_with_fragment) {
  const sourcemeta::core::URI uri{"https://example.com#foo"};
  EXPECT_FALSE(uri.empty());
  EXPECT_EQ(uri.recompose(), "https://example.com#foo");
}
