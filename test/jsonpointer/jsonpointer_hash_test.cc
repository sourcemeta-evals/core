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
  const sourcemeta::core::Pointer pointer{"foo", "bar", 1};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_equal_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", 1};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", 1};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(pointer1, pointer2);
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_different_pointers_different_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(pointer1, pointer2);
  // Different pointers should typically have different hashes
  // (though collisions are theoretically possible)
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_different_sizes) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(pointer1, pointer2);
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
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
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weak_pointer_single_index) {
  const sourcemeta::core::WeakPointer pointer{5};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weak_pointer_multiple_tokens) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              1};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weak_pointer_equal_pointers_same_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar),
                                               1};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar),
                                               1};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(pointer1, pointer2);
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weak_pointer_different_pointers_different_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(baz)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(pointer1, pointer2);
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, unordered_set_pointer) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"baz", "qux"};
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
  const sourcemeta::core::Pointer pointer2{"baz", "qux"};

  map[pointer1] = 42;
  map[pointer2] = 100;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 42);
  EXPECT_EQ(map[pointer2], 100);
}

TEST(JSONPointer_hash, unordered_set_weak_pointer) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const std::string qux = "qux";

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz), std::cref(qux)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo), std::cref(bar)};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(pointer1) != set.end());
  EXPECT_TRUE(set.find(pointer2) != set.end());
  EXPECT_TRUE(set.find(pointer3) != set.end());
}

TEST(JSONPointer_hash, unordered_map_weak_pointer) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const std::string qux = "qux";

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz), std::cref(qux)};

  map[pointer1] = 42;
  map[pointer2] = 100;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 42);
  EXPECT_EQ(map[pointer2], 100);
}

TEST(JSONPointer_hash, pointer_long_path_sampling) {
  // Test that long paths are hashed efficiently (O(1))
  // by sampling first, middle, and last tokens
  const sourcemeta::core::Pointer pointer1{"a", "b", "c", "d", "e",
                                           "f", "g", "h", "i", "j"};
  const sourcemeta::core::Pointer pointer2{"a", "b", "c", "d", "e",
                                           "f", "g", "h", "i", "k"};
  const std::hash<sourcemeta::core::Pointer> hasher;

  // Different last token should produce different hash
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_mixed_tokens) {
  // Test with mixed property and index tokens
  const sourcemeta::core::Pointer pointer1{"foo", 0, "bar", 1, "baz"};
  const sourcemeta::core::Pointer pointer2{"foo", 0, "bar", 2, "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;

  // Different middle token should produce different hash
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}
