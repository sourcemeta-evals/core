#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <functional> // std::cref
#include <string>     // std::string
#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_empty) {
  const sourcemeta::core::Pointer pointer;
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash{hasher(pointer)};
  EXPECT_EQ(hash, 0xcbf29ce484222325ULL);
}

TEST(JSONPointer_hash, pointer_property_single) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash{hasher(pointer)};
  EXPECT_GT(hash, 0);
}

TEST(JSONPointer_hash, pointer_property_multiple) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash{hasher(pointer)};
  EXPECT_GT(hash, 0);
}

TEST(JSONPointer_hash, pointer_index_single) {
  const sourcemeta::core::Pointer pointer{0};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash{hasher(pointer)};
  EXPECT_GT(hash, 0);
}

TEST(JSONPointer_hash, pointer_index_multiple) {
  const sourcemeta::core::Pointer pointer{1, 2, 3};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash{hasher(pointer)};
  EXPECT_GT(hash, 0);
}

TEST(JSONPointer_hash, pointer_mixed) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash{hasher(pointer)};
  EXPECT_GT(hash, 0);
}

TEST(JSONPointer_hash, pointer_different_hashes) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", "qux"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1{hasher(pointer1)};
  const auto hash2{hasher(pointer2)};
  EXPECT_NE(hash1, hash2);
}

TEST(JSONPointer_hash, pointer_equal_same_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1{hasher(pointer1)};
  const auto hash2{hasher(pointer2)};
  EXPECT_EQ(hash1, hash2);
}

TEST(JSONPointer_hash, pointer_unordered_set_insert_find) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", "qux"};
  const sourcemeta::core::Pointer pointer3{1, 2, 3};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 3);
  EXPECT_TRUE(set.find(pointer1) != set.end());
  EXPECT_TRUE(set.find(pointer2) != set.end());
  EXPECT_TRUE(set.find(pointer3) != set.end());

  const sourcemeta::core::Pointer pointer4{"nonexistent"};
  EXPECT_TRUE(set.find(pointer4) == set.end());
}

TEST(JSONPointer_hash, pointer_unordered_set_duplicate) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer{"foo", "bar"};

  set.insert(pointer);
  set.insert(pointer);

  EXPECT_EQ(set.size(), 1);
}

TEST(JSONPointer_hash, pointer_unordered_set_erase) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer{"foo", "bar"};

  set.insert(pointer);
  EXPECT_EQ(set.size(), 1);

  set.erase(pointer);
  EXPECT_EQ(set.size(), 0);
}

TEST(JSONPointer_hash, pointer_unordered_map_insert_get) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const sourcemeta::core::Pointer pointer3{0};

  map[pointer1] = 1;
  map[pointer2] = 2;
  map[pointer3] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[pointer1], 1);
  EXPECT_EQ(map[pointer2], 2);
  EXPECT_EQ(map[pointer3], 3);
}

TEST(JSONPointer_hash, pointer_unordered_map_update) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer{"foo", "bar"};

  map[pointer] = 1;
  EXPECT_EQ(map[pointer], 1);

  map[pointer] = 2;
  EXPECT_EQ(map[pointer], 2);
  EXPECT_EQ(map.size(), 1);
}

TEST(JSONPointer_hash, weakpointer_empty) {
  const sourcemeta::core::WeakPointer pointer;
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash{hasher(pointer)};
  EXPECT_EQ(hash, 0xcbf29ce484222325ULL);
}

TEST(JSONPointer_hash, weakpointer_property_single) {
  const std::string foo{"foo"};
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash{hasher(pointer)};
  EXPECT_GT(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_property_multiple) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash{hasher(pointer)};
  EXPECT_GT(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_index_single) {
  const sourcemeta::core::WeakPointer pointer{0};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash{hasher(pointer)};
  EXPECT_GT(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_index_multiple) {
  const sourcemeta::core::WeakPointer pointer{1, 2, 3};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash{hasher(pointer)};
  EXPECT_GT(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_mixed) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), 1,
                                              std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash{hasher(pointer)};
  EXPECT_GT(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_different_hashes) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};
  const std::string qux{"qux"};
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar),
                                               std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar),
                                               std::cref(qux)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash1{hasher(pointer1)};
  const auto hash2{hasher(pointer2)};
  EXPECT_NE(hash1, hash2);
}

TEST(JSONPointer_hash, weakpointer_equal_same_hash) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar),
                                               std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar),
                                               std::cref(baz)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash1{hasher(pointer1)};
  const auto hash2{hasher(pointer2)};
  EXPECT_EQ(hash1, hash2);
}

TEST(JSONPointer_hash, weakpointer_unordered_set_insert_find) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};
  const std::string qux{"qux"};
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar),
                                               std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar),
                                               std::cref(qux)};
  const sourcemeta::core::WeakPointer pointer3{1, 2, 3};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 3);
  EXPECT_TRUE(set.find(pointer1) != set.end());
  EXPECT_TRUE(set.find(pointer2) != set.end());
  EXPECT_TRUE(set.find(pointer3) != set.end());

  const std::string nonexistent{"nonexistent"};
  const sourcemeta::core::WeakPointer pointer4{std::cref(nonexistent)};
  EXPECT_TRUE(set.find(pointer4) == set.end());
}

TEST(JSONPointer_hash, weakpointer_unordered_set_duplicate) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar)};

  set.insert(pointer);
  set.insert(pointer);

  EXPECT_EQ(set.size(), 1);
}

TEST(JSONPointer_hash, weakpointer_unordered_set_erase) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar)};

  set.insert(pointer);
  EXPECT_EQ(set.size(), 1);

  set.erase(pointer);
  EXPECT_EQ(set.size(), 0);
}

TEST(JSONPointer_hash, weakpointer_unordered_map_insert_get) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer3{0};

  map[pointer1] = 1;
  map[pointer2] = 2;
  map[pointer3] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[pointer1], 1);
  EXPECT_EQ(map[pointer2], 2);
  EXPECT_EQ(map[pointer3], 3);
}

TEST(JSONPointer_hash, weakpointer_unordered_map_update) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar)};

  map[pointer] = 1;
  EXPECT_EQ(map[pointer], 1);

  map[pointer] = 2;
  EXPECT_EQ(map[pointer], 2);
  EXPECT_EQ(map.size(), 1);
}

TEST(JSONPointer_hash, pointer_size_affects_hash) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1{hasher(pointer1)};
  const auto hash2{hasher(pointer2)};
  EXPECT_NE(hash1, hash2);
}

TEST(JSONPointer_hash, weakpointer_size_affects_hash) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash1{hasher(pointer1)};
  const auto hash2{hasher(pointer2)};
  EXPECT_NE(hash1, hash2);
}
