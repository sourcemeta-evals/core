#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set

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
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  set.insert(sourcemeta::core::Pointer{"foo", "baz"});
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", "bar"}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", "baz"}));
}

TEST(JSONPointer_hash, pointer_unordered_set_mixed_tokens) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{"foo", 1, "bar"});
  set.insert(sourcemeta::core::Pointer{"foo", 2, "bar"});
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", 1, "bar"}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", 2, "bar"}));
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map[sourcemeta::core::Pointer{"foo"}] = 1;
  map[sourcemeta::core::Pointer{"bar"}] = 2;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo"}), 1);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"bar"}), 2);
}

TEST(JSONPointer_hash, pointer_equal_pointers_same_hash) {
  const std::hash<sourcemeta::core::Pointer> hasher;
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", 1, "bar"};
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_empty_hash_is_zero) {
  const std::hash<sourcemeta::core::Pointer> hasher;
  const sourcemeta::core::Pointer pointer;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weak_pointer_unordered_set_empty) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  set.emplace();
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.contains(sourcemeta::core::WeakPointer{}));
}

TEST(JSONPointer_hash, weak_pointer_unordered_set_single_property) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo)});
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.contains(sourcemeta::core::WeakPointer{std::cref(foo)}));
  EXPECT_FALSE(set.contains(sourcemeta::core::WeakPointer{std::cref(bar)}));
}

TEST(JSONPointer_hash, weak_pointer_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  map[sourcemeta::core::WeakPointer{std::cref(foo)}] = 1;
  map[sourcemeta::core::WeakPointer{std::cref(bar)}] = 2;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(foo)}), 1);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(bar)}), 2);
}

TEST(JSONPointer_hash, weak_pointer_equal_pointers_same_hash) {
  const std::string foo = "foo";
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(foo)};
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, weak_pointer_empty_hash_is_zero) {
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const sourcemeta::core::WeakPointer pointer;
  EXPECT_EQ(hasher(pointer), 0);
}
