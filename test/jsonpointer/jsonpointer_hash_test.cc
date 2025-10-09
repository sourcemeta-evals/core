#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, unordered_map_basic) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  const sourcemeta::core::Pointer ptr1{"foo", "bar"};
  const sourcemeta::core::Pointer ptr2{"baz"};

  map[ptr1] = 1;
  map[ptr2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[ptr1], 1);
  EXPECT_EQ(map[ptr2], 2);
}

TEST(JSONPointer_hash, unordered_map_with_indices) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  const sourcemeta::core::Pointer ptr1{"foo", 0, "bar"};
  const sourcemeta::core::Pointer ptr2{"foo", 1, "bar"};

  map[ptr1] = 10;
  map[ptr2] = 20;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[ptr1], 10);
  EXPECT_EQ(map[ptr2], 20);
}

TEST(JSONPointer_hash, unordered_set_basic) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  const sourcemeta::core::Pointer ptr1{"foo", "bar"};
  const sourcemeta::core::Pointer ptr2{"baz"};
  const sourcemeta::core::Pointer ptr3{"foo", "bar"};

  set.insert(ptr1);
  set.insert(ptr2);
  set.insert(ptr3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.count(ptr1) > 0);
  EXPECT_TRUE(set.count(ptr2) > 0);
  EXPECT_TRUE(set.count(ptr3) > 0);
}

TEST(JSONPointer_hash, hash_consistency) {
  const sourcemeta::core::Pointer ptr1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer ptr2{"foo", "bar", "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(ptr1), hasher(ptr2));
}

TEST(JSONPointer_hash, hash_empty_pointer) {
  const sourcemeta::core::Pointer empty1;
  const sourcemeta::core::Pointer empty2;

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(empty1), hasher(empty2));
  EXPECT_EQ(hasher(empty1), 0);
}

TEST(JSONPointer_hash, hash_single_token_property) {
  const sourcemeta::core::Pointer ptr{"foo"};

  std::hash<sourcemeta::core::Pointer> hasher;
  auto hash_value = hasher(ptr);

  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, hash_single_token_index) {
  const sourcemeta::core::Pointer ptr{42};

  std::hash<sourcemeta::core::Pointer> hasher;
  auto hash_value = hasher(ptr);

  EXPECT_EQ(hash_value, 42);
}

TEST(JSONPointer_hash, hash_differentiation) {
  const sourcemeta::core::Pointer ptr1{"foo"};
  const sourcemeta::core::Pointer ptr2{"bar"};

  std::hash<sourcemeta::core::Pointer> hasher;

  EXPECT_NE(hasher(ptr1), hasher(ptr2));
}

TEST(JSONWeakPointer_hash, unordered_map_basic) {
  static const std::string foo = "foo";
  static const std::string bar = "bar";
  static const std::string baz = "baz";

  std::unordered_map<sourcemeta::core::WeakPointer, int> map;

  const sourcemeta::core::WeakPointer ptr1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer ptr2{std::cref(baz)};

  map[ptr1] = 1;
  map[ptr2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[ptr1], 1);
  EXPECT_EQ(map[ptr2], 2);
}

TEST(JSONWeakPointer_hash, unordered_set_basic) {
  static const std::string foo = "foo";
  static const std::string bar = "bar";
  static const std::string baz = "baz";

  std::unordered_set<sourcemeta::core::WeakPointer> set;

  const sourcemeta::core::WeakPointer ptr1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer ptr2{std::cref(baz)};
  const sourcemeta::core::WeakPointer ptr3{std::cref(foo), std::cref(bar)};

  set.insert(ptr1);
  set.insert(ptr2);
  set.insert(ptr3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.count(ptr1) > 0);
  EXPECT_TRUE(set.count(ptr2) > 0);
  EXPECT_TRUE(set.count(ptr3) > 0);
}

TEST(JSONWeakPointer_hash, hash_consistency) {
  static const std::string foo = "foo";
  static const std::string bar = "bar";
  static const std::string baz = "baz";

  const sourcemeta::core::WeakPointer ptr1{std::cref(foo), std::cref(bar),
                                           std::cref(baz)};
  const sourcemeta::core::WeakPointer ptr2{std::cref(foo), std::cref(bar),
                                           std::cref(baz)};

  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(ptr1), hasher(ptr2));
}

TEST(JSONWeakPointer_hash, hash_empty_pointer) {
  const sourcemeta::core::WeakPointer empty1;
  const sourcemeta::core::WeakPointer empty2;

  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(empty1), hasher(empty2));
  EXPECT_EQ(hasher(empty1), 0);
}
