#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_unordered_set_empty) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.emplace();
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{}));
}

TEST(JSONPointer_hash, pointer_unordered_set_single_property) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.emplace(std::initializer_list<sourcemeta::core::Pointer::Token>{"foo"});
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo"}));
  EXPECT_FALSE(set.contains(sourcemeta::core::Pointer{"bar"}));
}

TEST(JSONPointer_hash, pointer_unordered_set_single_index) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.emplace(std::initializer_list<sourcemeta::core::Pointer::Token>{0});
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{0}));
  EXPECT_FALSE(set.contains(sourcemeta::core::Pointer{1}));
}

TEST(JSONPointer_hash, pointer_unordered_set_multiple) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.emplace(
      std::initializer_list<sourcemeta::core::Pointer::Token>{"foo", 1});
  set.emplace(
      std::initializer_list<sourcemeta::core::Pointer::Token>{"bar", 2});
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", 1}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"bar", 2}));
  EXPECT_FALSE(set.contains(sourcemeta::core::Pointer{"baz", 3}));
}

TEST(JSONPointer_hash, pointer_unordered_set_duplicates) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.emplace(
      std::initializer_list<sourcemeta::core::Pointer::Token>{"foo", "bar"});
  set.emplace(
      std::initializer_list<sourcemeta::core::Pointer::Token>{"foo", "bar"});
  EXPECT_EQ(set.size(), 1);
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map.emplace(sourcemeta::core::Pointer{"foo", "bar"}, 1);
  map.emplace(sourcemeta::core::Pointer{"baz"}, 2);
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo", "bar"}), 1);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"baz"}), 2);
}

TEST(JSONPointer_hash, pointer_equal_pointers_same_hash) {
  const std::hash<sourcemeta::core::Pointer> hasher;
  const sourcemeta::core::Pointer a{"foo", 1, "bar"};
  const sourcemeta::core::Pointer b{"foo", 1, "bar"};
  EXPECT_EQ(hasher(a), hasher(b));
}

TEST(JSONPointer_hash, pointer_empty_hash) {
  const std::hash<sourcemeta::core::Pointer> hasher;
  const sourcemeta::core::Pointer a;
  const sourcemeta::core::Pointer b;
  EXPECT_EQ(hasher(a), hasher(b));
  EXPECT_EQ(hasher(a), 0);
}

TEST(JSONPointer_hash, weak_pointer_unordered_set_empty) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  set.emplace();
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.contains(sourcemeta::core::WeakPointer{}));
}

TEST(JSONPointer_hash, weak_pointer_unordered_set_single_property) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  set.emplace(std::initializer_list<sourcemeta::core::WeakPointer::Token>{
      std::cref(foo)});
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.contains(sourcemeta::core::WeakPointer{std::cref(foo)}));
  EXPECT_FALSE(set.contains(sourcemeta::core::WeakPointer{std::cref(bar)}));
}

TEST(JSONPointer_hash, weak_pointer_unordered_set_single_index) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  set.emplace(std::initializer_list<sourcemeta::core::WeakPointer::Token>{0});
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.contains(sourcemeta::core::WeakPointer{0}));
  EXPECT_FALSE(set.contains(sourcemeta::core::WeakPointer{1}));
}

TEST(JSONPointer_hash, weak_pointer_unordered_map) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  map.emplace(sourcemeta::core::WeakPointer{std::cref(foo)}, 1);
  map.emplace(sourcemeta::core::WeakPointer{std::cref(bar)}, 2);
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(foo)}), 1);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(bar)}), 2);
}

TEST(JSONPointer_hash, weak_pointer_equal_pointers_same_hash) {
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const sourcemeta::core::WeakPointer a{std::cref(foo), 1, std::cref(bar)};
  const sourcemeta::core::WeakPointer b{std::cref(foo), 1, std::cref(bar)};
  EXPECT_EQ(hasher(a), hasher(b));
}

TEST(JSONPointer_hash, pointer_unordered_set_many_tokens) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.emplace(std::initializer_list<sourcemeta::core::Pointer::Token>{
      "a", "b", "c", "d", "e"});
  set.emplace(std::initializer_list<sourcemeta::core::Pointer::Token>{
      "a", "b", "x", "d", "e"});
  EXPECT_EQ(set.size(), 2);
}
