#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <functional>    // std::hash
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.emplace(sourcemeta::core::Pointer{"foo", "bar"});
  set.emplace(sourcemeta::core::Pointer{"baz"});
  set.emplace(sourcemeta::core::Pointer{0, 1});
  EXPECT_EQ(set.size(), 3);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", "bar"}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"baz"}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{0, 1}));
  EXPECT_FALSE(set.contains(sourcemeta::core::Pointer{"other"}));
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map.emplace(sourcemeta::core::Pointer{"foo"}, 1);
  map.emplace(sourcemeta::core::Pointer{"bar"}, 2);
  map.emplace(sourcemeta::core::Pointer{0}, 3);
  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo"}), 1);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"bar"}), 2);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{0}), 3);
}

TEST(JSONPointer_hash, pointer_empty) {
  const std::hash<sourcemeta::core::Pointer> hasher;
  const sourcemeta::core::Pointer pointer;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_equal_pointers_same_hash) {
  const std::hash<sourcemeta::core::Pointer> hasher;
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", 1, "bar"};
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_different_pointers_different_hash) {
  const std::hash<sourcemeta::core::Pointer> hasher;
  const sourcemeta::core::Pointer pointer_1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer_2{"baz", "qux"};
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_single_property) {
  const std::hash<sourcemeta::core::Pointer> hasher;
  const sourcemeta::core::Pointer pointer{"foo"};
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_single_index) {
  const std::hash<sourcemeta::core::Pointer> hasher;
  const sourcemeta::core::Pointer pointer{5};
  EXPECT_EQ(hasher(pointer), 5);
}

TEST(JSONPointer_hash, weak_pointer_unordered_set) {
  static const std::string foo = "foo";
  static const std::string bar = "bar";
  static const std::string baz = "baz";

  std::unordered_set<sourcemeta::core::WeakPointer> set;
  set.emplace(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  set.emplace(sourcemeta::core::WeakPointer{std::cref(baz)});
  set.emplace(sourcemeta::core::WeakPointer{0, 1});
  EXPECT_EQ(set.size(), 3);
  EXPECT_TRUE(set.contains(
      sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)}));
  EXPECT_TRUE(set.contains(sourcemeta::core::WeakPointer{std::cref(baz)}));
  EXPECT_TRUE(set.contains(sourcemeta::core::WeakPointer{0, 1}));
}

TEST(JSONPointer_hash, weak_pointer_unordered_map) {
  static const std::string foo = "foo";
  static const std::string bar = "bar";

  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  map.emplace(sourcemeta::core::WeakPointer{std::cref(foo)}, 1);
  map.emplace(sourcemeta::core::WeakPointer{std::cref(bar)}, 2);
  map.emplace(sourcemeta::core::WeakPointer{0}, 3);
  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(foo)}), 1);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(bar)}), 2);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{0}), 3);
}

TEST(JSONPointer_hash, weak_pointer_empty) {
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const sourcemeta::core::WeakPointer pointer;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weak_pointer_equal_pointers_same_hash) {
  static const std::string foo = "foo";
  static const std::string bar = "bar";

  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), 1,
                                                std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(foo), 1,
                                                std::cref(bar)};
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, weak_pointer_single_index) {
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const sourcemeta::core::WeakPointer pointer{5};
  EXPECT_EQ(hasher(pointer), 5);
}

TEST(JSONPointer_hash, pointer_unordered_set_duplicate) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.emplace(sourcemeta::core::Pointer{"foo", "bar"});
  set.emplace(sourcemeta::core::Pointer{"foo", "bar"});
  EXPECT_EQ(set.size(), 1);
}
