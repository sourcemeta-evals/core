#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_empty) {
  const sourcemeta::core::Pointer pointer;
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_single_property) {
  const sourcemeta::core::Pointer pointer{"foo"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_single_index) {
  const sourcemeta::core::Pointer pointer{5};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_EQ(hash_value, 5);
}

TEST(JSONPointer_hash, pointer_two_properties) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_multiple_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_mixed_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_equal_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_different_pointers_different_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> pointer_set;

  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const sourcemeta::core::Pointer pointer3{"foo"};

  pointer_set.insert(pointer1);
  pointer_set.insert(pointer2);
  pointer_set.insert(pointer3);

  EXPECT_EQ(pointer_set.size(), 2);
  EXPECT_TRUE(pointer_set.find(pointer1) != pointer_set.end());
  EXPECT_TRUE(pointer_set.find(pointer2) != pointer_set.end());
  EXPECT_TRUE(pointer_set.find(pointer3) != pointer_set.end());
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> pointer_map;

  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const sourcemeta::core::Pointer pointer3{"baz"};

  pointer_map[pointer1] = 1;
  pointer_map[pointer2] = 2;
  pointer_map[pointer3] = 3;

  EXPECT_EQ(pointer_map.size(), 3);
  EXPECT_EQ(pointer_map[pointer1], 1);
  EXPECT_EQ(pointer_map[pointer2], 2);
  EXPECT_EQ(pointer_map[pointer3], 3);
}

TEST(JSONPointer_hash, weakpointer_empty) {
  const sourcemeta::core::WeakPointer pointer;
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weakpointer_single_property) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, weakpointer_single_index) {
  const sourcemeta::core::WeakPointer pointer{5};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_EQ(hash_value, 5);
}

TEST(JSONPointer_hash, weakpointer_two_properties) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, weakpointer_multiple_tokens) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, weakpointer_equal_pointers_same_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_unordered_set) {
  std::unordered_set<sourcemeta::core::WeakPointer> pointer_set;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo)};

  pointer_set.insert(pointer1);
  pointer_set.insert(pointer2);
  pointer_set.insert(pointer3);

  EXPECT_EQ(pointer_set.size(), 2);
  EXPECT_TRUE(pointer_set.find(pointer1) != pointer_set.end());
  EXPECT_TRUE(pointer_set.find(pointer2) != pointer_set.end());
  EXPECT_TRUE(pointer_set.find(pointer3) != pointer_set.end());
}

TEST(JSONPointer_hash, weakpointer_unordered_map) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> pointer_map;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(baz)};

  pointer_map[pointer1] = 1;
  pointer_map[pointer2] = 2;
  pointer_map[pointer3] = 3;

  EXPECT_EQ(pointer_map.size(), 3);
  EXPECT_EQ(pointer_map[pointer1], 1);
  EXPECT_EQ(pointer_map[pointer2], 2);
  EXPECT_EQ(pointer_map[pointer3], 3);
}

TEST(JSONPointer_hash, pointer_and_weakpointer_same_hash) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer weak_pointer{std::cref(foo),
                                                   std::cref(bar)};

  std::hash<sourcemeta::core::Pointer> pointer_hasher;
  std::hash<sourcemeta::core::WeakPointer> weak_pointer_hasher;

  EXPECT_EQ(pointer_hasher(pointer), weak_pointer_hasher(weak_pointer));
}
