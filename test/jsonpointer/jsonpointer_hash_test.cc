#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <functional>
#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_empty) {
  const sourcemeta::core::Pointer pointer;
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_single_property) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value{hasher(pointer)};
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_single_index) {
  const sourcemeta::core::Pointer pointer{42};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value{hasher(pointer)};
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_multiple_properties) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value{hasher(pointer)};
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_multiple_indices) {
  const sourcemeta::core::Pointer pointer{0, 1, 2};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value{hasher(pointer)};
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_mixed_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar", 2};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value{hasher(pointer)};
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_equal_pointers_equal_hashes) {
  const sourcemeta::core::Pointer pointer_1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_different_pointers_different_hashes) {
  const sourcemeta::core::Pointer pointer_1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer_1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer_2{"baz"};
  const sourcemeta::core::Pointer pointer_3{0, 1};

  set.insert(pointer_1);
  set.insert(pointer_2);
  set.insert(pointer_3);

  EXPECT_EQ(set.size(), 3);
  EXPECT_TRUE(set.find(pointer_1) != set.end());
  EXPECT_TRUE(set.find(pointer_2) != set.end());
  EXPECT_TRUE(set.find(pointer_3) != set.end());
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer_1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer_2{"baz"};
  const sourcemeta::core::Pointer pointer_3{0, 1};

  map[pointer_1] = 1;
  map[pointer_2] = 2;
  map[pointer_3] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map.at(pointer_1), 1);
  EXPECT_EQ(map.at(pointer_2), 2);
  EXPECT_EQ(map.at(pointer_3), 3);
}

TEST(JSONPointer_hash, weakpointer_empty) {
  const sourcemeta::core::WeakPointer pointer;
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weakpointer_single_property) {
  const std::string foo{"foo"};
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value{hasher(pointer)};
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, weakpointer_single_index) {
  const sourcemeta::core::WeakPointer pointer{42};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value{hasher(pointer)};
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, weakpointer_multiple_properties) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value{hasher(pointer)};
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, weakpointer_multiple_indices) {
  const sourcemeta::core::WeakPointer pointer{0, 1, 2};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value{hasher(pointer)};
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, weakpointer_mixed_tokens) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), 1, std::cref(bar),
                                              2};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value{hasher(pointer)};
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, weakpointer_equal_pointers_equal_hashes) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(foo), std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, weakpointer_different_pointers_different_hashes) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(foo), std::cref(baz)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, weakpointer_unordered_set) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};

  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer_3{0, 1};

  set.insert(pointer_1);
  set.insert(pointer_2);
  set.insert(pointer_3);

  EXPECT_EQ(set.size(), 3);
  EXPECT_TRUE(set.find(pointer_1) != set.end());
  EXPECT_TRUE(set.find(pointer_2) != set.end());
  EXPECT_TRUE(set.find(pointer_3) != set.end());
}

TEST(JSONPointer_hash, weakpointer_unordered_map) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};

  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer_3{0, 1};

  map[pointer_1] = 1;
  map[pointer_2] = 2;
  map[pointer_3] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map.at(pointer_1), 1);
  EXPECT_EQ(map.at(pointer_2), 2);
  EXPECT_EQ(map.at(pointer_3), 3);
}

TEST(JSONPointer_hash, pointer_and_weakpointer_compatible_hashes) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::WeakPointer weak_pointer{std::cref(foo),
                                                   std::cref(bar)};

  const std::hash<sourcemeta::core::Pointer> pointer_hasher;
  const std::hash<sourcemeta::core::WeakPointer> weak_pointer_hasher;

  EXPECT_EQ(pointer_hasher(pointer), weak_pointer_hasher(weak_pointer));
}
