#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_is_hashable) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, weakpointer_is_hashable) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_hash_consistency) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1 = hasher(pointer);
  const auto hash2 = hasher(pointer);
  EXPECT_EQ(hash1, hash2);
}

TEST(JSONPointer_hash, weakpointer_hash_consistency) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash1 = hasher(pointer);
  const auto hash2 = hasher(pointer);
  EXPECT_EQ(hash1, hash2);
}

TEST(JSONPointer_hash, pointer_equal_pointers_equal_hashes) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_equal_pointers_equal_hashes) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_empty_hash) {
  const sourcemeta::core::Pointer pointer;
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weakpointer_empty_hash) {
  const sourcemeta::core::WeakPointer pointer;
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_single_token) {
  const sourcemeta::core::Pointer pointer{"foo"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, weakpointer_single_token) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_two_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_three_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_mixed_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 0, "bar", 1, "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const sourcemeta::core::Pointer pointer3{"baz"};

  map[pointer1] = 1;
  map[pointer2] = 2;
  map[pointer3] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[pointer1], 1);
  EXPECT_EQ(map[pointer2], 2);
  EXPECT_EQ(map[pointer3], 3);
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const sourcemeta::core::Pointer pointer3{"foo"};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.count(pointer1) > 0);
  EXPECT_TRUE(set.count(pointer2) > 0);
  EXPECT_TRUE(set.count(pointer3) > 0);
}

TEST(JSONPointer_hash, weakpointer_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(baz)};

  map[pointer1] = 1;
  map[pointer2] = 2;
  map[pointer3] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[pointer1], 1);
  EXPECT_EQ(map[pointer2], 2);
  EXPECT_EQ(map[pointer3], 3);
}

TEST(JSONPointer_hash, weakpointer_unordered_set) {
  const std::string foo = "foo";
  const std::string bar = "bar";

  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo)};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.count(pointer1) > 0);
  EXPECT_TRUE(set.count(pointer2) > 0);
  EXPECT_TRUE(set.count(pointer3) > 0);
}

TEST(JSONPointer_hash, pointer_different_hashes) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_index_only) {
  const sourcemeta::core::Pointer pointer{0, 1, 2};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}
