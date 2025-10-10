#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_in_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"baz"};

  map[pointer1] = 42;
  map[pointer2] = 100;

  EXPECT_EQ(map[pointer1], 42);
  EXPECT_EQ(map[pointer2], 100);
  EXPECT_EQ(map.size(), 2);
}

TEST(JSONPointer_hash, pointer_in_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"baz"};
  const sourcemeta::core::Pointer pointer3{"foo", "bar"};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.count(pointer1) > 0);
  EXPECT_TRUE(set.count(pointer2) > 0);
  EXPECT_TRUE(set.count(pointer3) > 0);
}

TEST(JSONPointer_hash, weakpointer_in_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  std::unordered_map<sourcemeta::core::WeakPointer, int> map;

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz)};

  map[pointer1] = 42;
  map[pointer2] = 100;

  EXPECT_EQ(map[pointer1], 42);
  EXPECT_EQ(map[pointer2], 100);
  EXPECT_EQ(map.size(), 2);
}

TEST(JSONPointer_hash, weakpointer_in_unordered_set) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  std::unordered_set<sourcemeta::core::WeakPointer> set;

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo), std::cref(bar)};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.count(pointer1) > 0);
  EXPECT_TRUE(set.count(pointer2) > 0);
  EXPECT_TRUE(set.count(pointer3) > 0);
}

TEST(JSONPointer_hash, equal_pointers_equal_hashes) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, hash_consistency) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;

  const auto hash1 = hasher(pointer);
  const auto hash2 = hasher(pointer);
  const auto hash3 = hasher(pointer);

  EXPECT_EQ(hash1, hash2);
  EXPECT_EQ(hash2, hash3);
}

TEST(JSONPointer_hash, empty_pointer_hashes_to_zero) {
  const sourcemeta::core::Pointer pointer;
  std::hash<sourcemeta::core::Pointer> hasher;

  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, different_pointers_different_hashes) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const sourcemeta::core::Pointer pointer3{"foo", "bar"};

  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1 = hasher(pointer1);
  const auto hash2 = hasher(pointer2);
  const auto hash3 = hasher(pointer3);

  EXPECT_NE(hash1, hash2);
  EXPECT_NE(hash1, hash3);
  EXPECT_NE(hash2, hash3);
}

TEST(JSONPointer_hash, pointer_with_indices) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  const sourcemeta::core::Pointer pointer1{"foo", 0, "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", 1, "bar"};

  map[pointer1] = 42;
  map[pointer2] = 100;

  EXPECT_EQ(map[pointer1], 42);
  EXPECT_EQ(map[pointer2], 100);
  EXPECT_EQ(map.size(), 2);
}

TEST(JSONPointer_hash, single_token_pointer) {
  const sourcemeta::core::Pointer pointer{"foo"};
  std::hash<sourcemeta::core::Pointer> hasher;

  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, two_token_pointer) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;

  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, many_token_pointer) {
  const sourcemeta::core::Pointer pointer{"a", "b", "c", "d", "e"};
  std::hash<sourcemeta::core::Pointer> hasher;

  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weakpointer_equal_hashes) {
  const std::string foo = "foo";
  const std::string bar = "bar";

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar)};

  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_empty_hashes_to_zero) {
  const sourcemeta::core::WeakPointer pointer;
  std::hash<sourcemeta::core::WeakPointer> hasher;

  EXPECT_EQ(hasher(pointer), 0);
}
