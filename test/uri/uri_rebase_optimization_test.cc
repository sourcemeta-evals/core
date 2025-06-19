#include <gtest/gtest.h>

#include <sourcemeta/core/uri.h>

TEST(URI_rebase_optimization, non_relative_uri_early_return) {
  sourcemeta::core::URI uri{"https://example.com/foo/bar/baz"};
  const sourcemeta::core::URI base{"https://another.com/test"};
  const sourcemeta::core::URI new_base{"/qux"};

  const auto original = uri.recompose();
  uri.rebase(base, new_base);

  EXPECT_EQ(uri.recompose(), original);
}

TEST(URI_rebase_optimization, relative_uri_gets_rebased) {
  sourcemeta::core::URI uri{"https://example.com/foo/bar/baz"};
  const sourcemeta::core::URI base{"https://example.com/foo"};
  const sourcemeta::core::URI new_base{"/qux"};

  uri.rebase(base, new_base);

  EXPECT_EQ(uri.recompose(), "/qux/bar/baz");
}

TEST(URI_rebase_optimization, absolute_uri_different_host) {
  sourcemeta::core::URI uri{"https://different.com/path"};
  const sourcemeta::core::URI base{"https://example.com/base"};
  const sourcemeta::core::URI new_base{"/new"};

  const auto original = uri.recompose();
  uri.rebase(base, new_base);

  EXPECT_EQ(uri.recompose(), original);
}
