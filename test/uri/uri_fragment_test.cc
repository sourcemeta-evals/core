#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_fragment, https_no_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/foo"};
  EXPECT_FALSE(uri.fragment().has_value());
}

TEST(URI_fragment, https_with_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/#foo"};
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "foo");
}

TEST(URI_fragment, https_with_empty_fragment) {
  const sourcemeta::core::URI uri{"https://example.com/foo#"};
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "");
}

TEST(URI_fragment, https_with_escaped_jsonpointer) {
  const sourcemeta::core::URI uri{"https://example.com/#/c%25d"};
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "/c%25d");
}

TEST(URI_fragment, invalid_fragment_with_pointer) {
  EXPECT_THROW(sourcemeta::core::URI{"#foo#/$defs/bar"},
               sourcemeta::core::URIParseError);
}

TEST(URI_fragment, urn) {
  const sourcemeta::core::URI uri{"urn:example:schema"};
  EXPECT_FALSE(uri.fragment().has_value());
}

TEST(URI_fragment, urn_with_fragment) {
  const sourcemeta::core::URI uri{"urn:example:schema#foo"};
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "foo");
}

TEST(URI_fragment, is_fragment_only_true_1) {
  const sourcemeta::core::URI uri{"#foo"};
  EXPECT_TRUE(uri.is_fragment_only());
}

TEST(URI_fragment, is_fragment_only_true_2) {
  // In this case it is not really a query, but part of the fragment
  const sourcemeta::core::URI uri{"#foo?bar=baz"};
  EXPECT_TRUE(uri.is_fragment_only());
  EXPECT_EQ(uri.fragment().value(), "foo?bar=baz");
}

TEST(URI_fragment, is_fragment_only_true_3) {
  const sourcemeta::core::URI uri{"#"};
  EXPECT_TRUE(uri.is_fragment_only());
}

TEST(URI_fragment, is_fragment_only_false_1) {
  const sourcemeta::core::URI uri{"foo#bar"};
  EXPECT_FALSE(uri.is_fragment_only());
}

TEST(URI_fragment, is_fragment_only_false_2) {
  const sourcemeta::core::URI uri{"example.com/#bar"};
  EXPECT_FALSE(uri.is_fragment_only());
}

TEST(URI_fragment, is_fragment_only_false_3) {
  const sourcemeta::core::URI uri{"urn:example:schema#foo"};
  EXPECT_FALSE(uri.is_fragment_only());
}

TEST(URI_fragment, set_fragment_basic) {
  sourcemeta::core::URI uri{"https://example.com/foo"};
  uri.fragment("bar");
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo#bar");
}

TEST(URI_fragment, set_fragment_move_semantics) {
  sourcemeta::core::URI uri{"https://example.com/foo"};
  std::string fragment_value{"bar"};
  uri.fragment(std::move(fragment_value));
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo#bar");
}

TEST(URI_fragment, set_fragment_empty_string) {
  sourcemeta::core::URI uri{"https://example.com/foo#existing"};
  uri.fragment("");
  EXPECT_FALSE(uri.fragment().has_value());
  EXPECT_EQ(uri.recompose(), "https://example.com/foo");
}

TEST(URI_fragment, set_fragment_with_hash_prefix) {
  sourcemeta::core::URI uri{"https://example.com/foo"};
  uri.fragment("#bar");
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo#bar");
}

TEST(URI_fragment, set_fragment_replace_existing) {
  sourcemeta::core::URI uri{"https://example.com/foo#old"};
  uri.fragment("new");
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "new");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo#new");
}

TEST(URI_fragment, set_fragment_chaining) {
  sourcemeta::core::URI uri{"https://example.com/foo"};
  uri.fragment("bar").fragment("baz");
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "baz");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo#baz");
}

TEST(URI_fragment, set_fragment_on_fragment_only_uri) {
  sourcemeta::core::URI uri{"#old"};
  uri.fragment("new");
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "new");
  EXPECT_EQ(uri.recompose(), "#new");
}
