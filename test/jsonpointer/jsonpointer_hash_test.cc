#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_empty_hash) {
  const sourcemeta::core::Pointer pointer;
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_single_property_hash) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_single_index_hash) {
  const sourcemeta::core::Pointer pointer{5};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_multiple_tokens_hash) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_equal_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_different_pointers_different_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", 2, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
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
  const sourcemeta::core::Pointer bar_key{"bar", 0};
  map[bar_key] = 2;
  map[sourcemeta::core::Pointer{"foo"}] = 3;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[sourcemeta::core::Pointer{"foo"}], 3);
  EXPECT_EQ(map[bar_key], 2);
}

TEST(JSONPointer_hash, pointer_complex_paths_in_set) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;
  pointers.insert(sourcemeta::core::Pointer{"a", "b", "c"});
  pointers.insert(sourcemeta::core::Pointer{"a", "b", "d"});
  pointers.insert(sourcemeta::core::Pointer{0, 1, 2});
  pointers.insert(sourcemeta::core::Pointer{"foo", 0, "bar", 1});
  EXPECT_EQ(pointers.size(), 4);
}

TEST(JSONPointer_hash, weakpointer_empty_hash) {
  const sourcemeta::core::WeakPointer pointer;
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weakpointer_single_property_hash) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_single_index_hash) {
  const sourcemeta::core::WeakPointer pointer{5};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_unordered_set) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  std::unordered_set<sourcemeta::core::WeakPointer> pointers;
  pointers.insert(sourcemeta::core::WeakPointer{std::cref(foo)});
  pointers.insert(sourcemeta::core::WeakPointer{std::cref(bar)});
  pointers.insert(sourcemeta::core::WeakPointer{std::cref(foo)});
  EXPECT_EQ(pointers.size(), 2);
}

TEST(JSONPointer_hash, weakpointer_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  map[sourcemeta::core::WeakPointer{std::cref(foo)}] = 1;
  const sourcemeta::core::WeakPointer bar_key{std::cref(bar), 0};
  map[bar_key] = 2;
  map[sourcemeta::core::WeakPointer{std::cref(foo)}] = 3;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[sourcemeta::core::WeakPointer{std::cref(foo)}], 3);
  EXPECT_EQ(map[bar_key], 2);
}
