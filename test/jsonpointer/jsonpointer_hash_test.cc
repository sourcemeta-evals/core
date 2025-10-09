#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <functional>
#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_in_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;

  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"baz", 1};
  const sourcemeta::core::Pointer pointer3{"foo", "bar"};

  pointers.insert(pointer1);
  pointers.insert(pointer2);
  pointers.insert(pointer3);

  EXPECT_EQ(pointers.size(), 2);
  EXPECT_TRUE(pointers.count(pointer1) > 0);
  EXPECT_TRUE(pointers.count(pointer2) > 0);
  EXPECT_TRUE(pointers.count(pointer3) > 0);
}

TEST(JSONPointer_hash, pointer_in_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"baz", 1};

  map[pointer1] = 42;
  map[pointer2] = 99;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 42);
  EXPECT_EQ(map[pointer2], 99);
}

TEST(JSONPointer_hash, weak_pointer_in_unordered_set) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};

  std::unordered_set<sourcemeta::core::WeakPointer> pointers;

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz), 1};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo), std::cref(bar)};

  pointers.insert(pointer1);
  pointers.insert(pointer2);
  pointers.insert(pointer3);

  EXPECT_EQ(pointers.size(), 2);
  EXPECT_TRUE(pointers.count(pointer1) > 0);
  EXPECT_TRUE(pointers.count(pointer2) > 0);
  EXPECT_TRUE(pointers.count(pointer3) > 0);
}

TEST(JSONPointer_hash, weak_pointer_in_unordered_map) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};

  std::unordered_map<sourcemeta::core::WeakPointer, int> map;

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz), 1};

  map[pointer1] = 42;
  map[pointer2] = 99;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 42);
  EXPECT_EQ(map[pointer2], 99);
}

TEST(JSONPointer_hash, consistency_same_pointer) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", 1};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", 1};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, consistency_different_pointers) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  hasher(pointer1);
  hasher(pointer2);
}

TEST(JSONPointer_hash, empty_pointer) {
  const sourcemeta::core::Pointer empty_pointer;

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(empty_pointer), 0);
}

TEST(JSONPointer_hash, single_token_property) {
  const sourcemeta::core::Pointer pointer{"foo"};

  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, single_token_index) {
  const sourcemeta::core::Pointer pointer{5};

  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_EQ(hash_value, 5);
}

TEST(JSONPointer_hash, mixed_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar", 2, "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}
