#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <functional>    // std::hash
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set

TEST(JSONPointer_hash, pointer_empty) {
  const sourcemeta::core::Pointer pointer;
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_single_property) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_single_index) {
  const sourcemeta::core::Pointer pointer{1};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_equal_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_different_pointers_likely_different_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer_2{"baz", "qux"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  // While hash collisions are possible, these should differ
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  set.insert(sourcemeta::core::Pointer{"baz"});
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", "bar"}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"baz"}));
  EXPECT_FALSE(set.contains(sourcemeta::core::Pointer{"qux"}));
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map[sourcemeta::core::Pointer{"foo"}] = 1;
  map[sourcemeta::core::Pointer{"bar"}] = 2;
  map[sourcemeta::core::Pointer{"foo"}] = 3;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo"}), 3);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"bar"}), 2);
}

TEST(JSONPointer_hash, weak_pointer_empty) {
  const sourcemeta::core::WeakPointer pointer;
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weak_pointer_single_property) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weak_pointer_single_index) {
  const sourcemeta::core::WeakPointer pointer{1};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weak_pointer_equal_pointers_same_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), 1,
                                                std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(foo), 1,
                                                std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, weak_pointer_unordered_set) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(baz)});
  EXPECT_EQ(set.size(), 2);
}

TEST(JSONPointer_hash, pointer_and_weak_pointer_consistent_hash) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer weak{std::cref(foo), 1, std::cref(bar)};
  const std::hash<sourcemeta::core::Pointer> pointer_hasher;
  const std::hash<sourcemeta::core::WeakPointer> weak_hasher;
  EXPECT_EQ(pointer_hasher(pointer), weak_hasher(weak));
}
