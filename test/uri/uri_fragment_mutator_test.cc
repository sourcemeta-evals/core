#include <gtest/gtest.h>
#include <sourcemeta/core/uri.h>
#include <string>

TEST(URI_fragment_mutator, set_fragment_basic) {
  sourcemeta::core::URI uri{"https://example.com/foo"};
  uri.fragment("bar");
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "bar");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo#bar");
}

TEST(URI_fragment_mutator, set_fragment_replace_existing) {
  sourcemeta::core::URI uri{"https://example.com/foo#old"};
  uri.fragment("new");
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "new");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo#new");
}

TEST(URI_fragment_mutator, set_fragment_empty) {
  sourcemeta::core::URI uri{"https://example.com/foo"};
  uri.fragment("");
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "");
  EXPECT_EQ(uri.recompose(), "https://example.com/foo#");
}

TEST(URI_fragment_mutator, set_fragment_clear_with_canonicalize) {
  sourcemeta::core::URI uri{"https://example.com/foo#old"};
  uri.fragment("");
  uri.canonicalize();
  EXPECT_FALSE(uri.fragment().has_value());
  EXPECT_EQ(uri.recompose(), "https://example.com/foo");
}

TEST(URI_fragment_mutator, set_fragment_move_rvalue) {
  sourcemeta::core::URI uri{"https://example.com/foo"};
  uri.fragment(std::string{"bar"});
  EXPECT_TRUE(uri.fragment().has_value());
  EXPECT_EQ(uri.fragment().value(), "bar");
}
