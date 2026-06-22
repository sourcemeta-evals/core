#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, token_property_hash) {
  const sourcemeta::core::Pointer::Token token{"foo"};
  std::hash<sourcemeta::core::Pointer::Token> hasher;
  const auto hash = hasher(token);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, token_index_hash) {
  const sourcemeta::core::Pointer::Token token{5};
  std::hash<sourcemeta::core::Pointer::Token> hasher;
  const auto hash = hasher(token);
  EXPECT_EQ(hash, 5);
}

TEST(JSONPointer_hash, token_index_zero_hash) {
  const sourcemeta::core::Pointer::Token token{0};
  std::hash<sourcemeta::core::Pointer::Token> hasher;
  const auto hash = hasher(token);
  EXPECT_EQ(hash, 0);
}

TEST(JSONPointer_hash, token_same_property_same_hash) {
  const sourcemeta::core::Pointer::Token token1{"foo"};
  const sourcemeta::core::Pointer::Token token2{"foo"};
  std::hash<sourcemeta::core::Pointer::Token> hasher;
  EXPECT_EQ(hasher(token1), hasher(token2));
}

TEST(JSONPointer_hash, token_different_property_different_hash) {
  const sourcemeta::core::Pointer::Token token1{"foo"};
  const sourcemeta::core::Pointer::Token token2{"bar"};
  std::hash<sourcemeta::core::Pointer::Token> hasher;
  EXPECT_NE(hasher(token1), hasher(token2));
}

TEST(JSONPointer_hash, pointer_empty_hash) {
  const sourcemeta::core::Pointer pointer;
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_single_token_hash) {
  const sourcemeta::core::Pointer pointer{"foo"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_multiple_tokens_hash) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_same_pointer_same_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_different_pointer_different_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;
  pointers.insert(sourcemeta::core::Pointer{"foo"});
  pointers.insert(sourcemeta::core::Pointer{"bar"});
  pointers.insert(sourcemeta::core::Pointer{"foo"});

  EXPECT_EQ(pointers.size(), 2);
  EXPECT_TRUE(pointers.contains(sourcemeta::core::Pointer{"foo"}));
  EXPECT_TRUE(pointers.contains(sourcemeta::core::Pointer{"bar"}));
  EXPECT_FALSE(pointers.contains(sourcemeta::core::Pointer{"baz"}));
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map[sourcemeta::core::Pointer{"foo"}] = 1;
  map[sourcemeta::core::Pointer{"bar"}] = 2;
  map[sourcemeta::core::Pointer{"foo", "baz"}] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo"}), 1);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"bar"}), 2);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo", "baz"}), 3);
}

TEST(JSONPointer_hash, weak_pointer_empty_hash) {
  const sourcemeta::core::WeakPointer pointer;
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weak_pointer_single_token_hash) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weak_pointer_unordered_set) {
  const std::string foo = "foo";
  const std::string bar = "bar";

  std::unordered_set<sourcemeta::core::WeakPointer> pointers;
  pointers.insert(sourcemeta::core::WeakPointer{std::cref(foo)});
  pointers.insert(sourcemeta::core::WeakPointer{std::cref(bar)});
  pointers.insert(sourcemeta::core::WeakPointer{std::cref(foo)});

  EXPECT_EQ(pointers.size(), 2);
  EXPECT_TRUE(pointers.contains(sourcemeta::core::WeakPointer{std::cref(foo)}));
  EXPECT_TRUE(pointers.contains(sourcemeta::core::WeakPointer{std::cref(bar)}));
}

TEST(JSONPointer_hash, weak_pointer_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  map[sourcemeta::core::WeakPointer{std::cref(foo)}] = 1;
  map[sourcemeta::core::WeakPointer{std::cref(bar)}] = 2;
  map[sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)}] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(foo)}), 1);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(bar)}), 2);
  EXPECT_EQ(
      map.at(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)}), 3);
}

TEST(JSONPointer_hash, token_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer::Token> tokens;
  tokens.insert(sourcemeta::core::Pointer::Token{"foo"});
  tokens.insert(sourcemeta::core::Pointer::Token{"bar"});
  tokens.insert(sourcemeta::core::Pointer::Token{5});
  tokens.insert(sourcemeta::core::Pointer::Token{"foo"});

  EXPECT_EQ(tokens.size(), 3);
  EXPECT_TRUE(tokens.contains(sourcemeta::core::Pointer::Token{"foo"}));
  EXPECT_TRUE(tokens.contains(sourcemeta::core::Pointer::Token{"bar"}));
  EXPECT_TRUE(tokens.contains(sourcemeta::core::Pointer::Token{5}));
}

TEST(JSONPointer_hash, token_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer::Token, int> map;
  map[sourcemeta::core::Pointer::Token{"foo"}] = 1;
  map[sourcemeta::core::Pointer::Token{"bar"}] = 2;
  map[sourcemeta::core::Pointer::Token{5}] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer::Token{"foo"}), 1);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer::Token{"bar"}), 2);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer::Token{5}), 3);
}
