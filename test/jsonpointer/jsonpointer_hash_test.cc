#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

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
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_single_index) {
  const sourcemeta::core::Pointer pointer{5};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_multiple_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_consistency) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1 = hasher(pointer);
  const auto hash2 = hasher(pointer);
  EXPECT_EQ(hash1, hash2);
}

TEST(JSONPointer_hash, pointer_different_values) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1 = hasher(pointer1);
  const auto hash2 = hasher(pointer2);
  EXPECT_NE(hash1, hash2);
}

TEST(JSONPointer_hash, pointer_equal_values_equal_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1 = hasher(pointer1);
  const auto hash2 = hasher(pointer2);
  EXPECT_EQ(hash1, hash2);
}

TEST(JSONPointer_hash, pointer_mixed_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 0, "bar", 1};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_empty) {
  const sourcemeta::core::WeakPointer pointer;
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weakpointer_single_property) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_single_index) {
  const sourcemeta::core::WeakPointer pointer{5};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_multiple_tokens) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_consistency) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash1 = hasher(pointer);
  const auto hash2 = hasher(pointer);
  EXPECT_EQ(hash1, hash2);
}

TEST(JSONPointer_hash, unordered_set_pointer) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "baz"};
  const sourcemeta::core::Pointer pointer3{"foo", "bar"};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(pointer1) != set.end());
  EXPECT_TRUE(set.find(pointer2) != set.end());
  EXPECT_TRUE(set.find(pointer3) != set.end());
}

TEST(JSONPointer_hash, unordered_map_pointer) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "baz"};

  map[pointer1] = 1;
  map[pointer2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 1);
  EXPECT_EQ(map[pointer2], 2);
}

TEST(JSONPointer_hash, unordered_set_weakpointer) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo), std::cref(bar)};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(pointer1) != set.end());
  EXPECT_TRUE(set.find(pointer2) != set.end());
  EXPECT_TRUE(set.find(pointer3) != set.end());
}

TEST(JSONPointer_hash, unordered_map_weakpointer) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(baz)};

  map[pointer1] = 1;
  map[pointer2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 1);
  EXPECT_EQ(map[pointer2], 2);
}
