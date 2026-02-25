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
  const auto result{hasher(pointer)};
  // Just check it produces a non-trivial hash
  EXPECT_NE(result, 0);
}

TEST(JSONPointer_hash, pointer_single_index) {
  const sourcemeta::core::Pointer pointer{2};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto result{hasher(pointer)};
  // Index 2 XOR'd with size 1 should be non-zero
  EXPECT_NE(result, 0);
}

TEST(JSONPointer_hash, pointer_equal_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_different_pointers_different_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"baz", 2, "qux"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  // Different pointers should (very likely) produce different hashes
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  set.insert(sourcemeta::core::Pointer{"baz"});
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", "bar"}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"baz"}));
  EXPECT_FALSE(set.contains(sourcemeta::core::Pointer{"qux"}));
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map[sourcemeta::core::Pointer{"foo"}] = 1;
  map[sourcemeta::core::Pointer{"bar", 0}] = 2;
  map[sourcemeta::core::Pointer{"foo"}] = 3;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo"}), 3);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"bar", 0}), 2);
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
  const auto result{hasher(pointer)};
  EXPECT_NE(result, 0);
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
  set.insert(sourcemeta::core::WeakPointer{std::cref(baz)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  EXPECT_EQ(set.size(), 2);
}

TEST(JSONPointer_hash, weak_pointer_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  map[sourcemeta::core::WeakPointer{std::cref(foo)}] = 1;
  map[sourcemeta::core::WeakPointer{std::cref(bar), 0}] = 2;
  map[sourcemeta::core::WeakPointer{std::cref(foo)}] = 3;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(foo)}), 3);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(bar), 0}), 2);
}

TEST(JSONPointer_hash, pointer_multiple_tokens_consistent) {
  const sourcemeta::core::Pointer pointer{"a", 0, "b", 1, "c"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  // Hash must be consistent across calls
  EXPECT_EQ(hasher(pointer), hasher(pointer));
}

TEST(JSONPointer_hash, pointer_index_zero) {
  const sourcemeta::core::Pointer pointer{0};
  const std::hash<sourcemeta::core::Pointer> hasher;
  // Index 0 XOR'd with size 1 should produce 1
  const auto result{hasher(pointer)};
  EXPECT_EQ(result, static_cast<std::size_t>(1));
}

TEST(JSONPointer_hash, pointer_index_one) {
  const sourcemeta::core::Pointer pointer_1{1};
  const std::hash<sourcemeta::core::Pointer> hasher;
  // Index 1 XOR'd with size 1 = 0
  const auto result{hasher(pointer_1)};
  EXPECT_EQ(result, static_cast<std::size_t>(0));
}
