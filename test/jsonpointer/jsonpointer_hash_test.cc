#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, empty_pointer) {
  const sourcemeta::core::Pointer pointer;
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_EQ(hash_value, 0);
}

TEST(JSONPointer_hash, single_property) {
  const sourcemeta::core::Pointer pointer{"foo"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, single_index) {
  const sourcemeta::core::Pointer pointer{5};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, multiple_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", 1, "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, same_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", 1};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", 1};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, different_pointers_different_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, different_sizes_different_hash) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, unordered_set_usage) {
  std::unordered_set<sourcemeta::core::Pointer> pointer_set;

  const sourcemeta::core::Pointer p1{"foo"};
  const sourcemeta::core::Pointer p2{"bar"};
  const sourcemeta::core::Pointer p3{"foo"};

  pointer_set.insert(p1);
  pointer_set.insert(p2);
  pointer_set.insert(p3);

  EXPECT_EQ(pointer_set.size(), 2);
  EXPECT_TRUE(pointer_set.count(p1) == 1);
  EXPECT_TRUE(pointer_set.count(p2) == 1);
}

TEST(JSONPointer_hash, unordered_map_usage) {
  std::unordered_map<sourcemeta::core::Pointer, int> pointer_map;

  const sourcemeta::core::Pointer p1{"foo", "bar"};
  const sourcemeta::core::Pointer p2{1, "baz"};

  pointer_map[p1] = 10;
  pointer_map[p2] = 20;

  EXPECT_EQ(pointer_map.size(), 2);
  EXPECT_EQ(pointer_map[p1], 10);
  EXPECT_EQ(pointer_map[p2], 20);
}

TEST(JSONPointer_hash, unordered_map_update) {
  std::unordered_map<sourcemeta::core::Pointer, int> pointer_map;

  const sourcemeta::core::Pointer p1{"foo"};

  pointer_map[p1] = 10;
  EXPECT_EQ(pointer_map[p1], 10);

  pointer_map[p1] = 20;
  EXPECT_EQ(pointer_map[p1], 20);
  EXPECT_EQ(pointer_map.size(), 1);
}

TEST(JSONWeakPointer_hash, empty_pointer) {
  const sourcemeta::core::WeakPointer pointer;
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_EQ(hash_value, 0);
}

TEST(JSONWeakPointer_hash, single_property) {
  static const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONWeakPointer_hash, single_index) {
  const sourcemeta::core::WeakPointer pointer{5};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONWeakPointer_hash, same_pointers_same_hash) {
  static const std::string foo = "foo";
  static const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONWeakPointer_hash, different_pointers_different_hash) {
  static const std::string foo = "foo";
  static const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONWeakPointer_hash, unordered_set_usage) {
  static const std::string foo = "foo";
  static const std::string bar = "bar";

  std::unordered_set<sourcemeta::core::WeakPointer> pointer_set;

  const sourcemeta::core::WeakPointer p1{std::cref(foo)};
  const sourcemeta::core::WeakPointer p2{std::cref(bar)};
  const sourcemeta::core::WeakPointer p3{std::cref(foo)};

  pointer_set.insert(p1);
  pointer_set.insert(p2);
  pointer_set.insert(p3);

  EXPECT_EQ(pointer_set.size(), 2);
}

TEST(JSONWeakPointer_hash, unordered_map_usage) {
  static const std::string foo = "foo";
  static const std::string bar = "bar";

  std::unordered_map<sourcemeta::core::WeakPointer, int> pointer_map;

  const sourcemeta::core::WeakPointer p1{std::cref(foo)};
  const sourcemeta::core::WeakPointer p2{std::cref(bar)};

  pointer_map[p1] = 10;
  pointer_map[p2] = 20;

  EXPECT_EQ(pointer_map.size(), 2);
  EXPECT_EQ(pointer_map[p1], 10);
  EXPECT_EQ(pointer_map[p2], 20);
}

TEST(JSONWeakPointer_hash, mixed_property_and_index) {
  static const std::string foo = "foo";
  static const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), 5,
                                              std::cref(bar)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}
