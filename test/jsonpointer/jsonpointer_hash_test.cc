#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_empty) {
  const sourcemeta::core::Pointer pointer;
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic for empty pointer
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, pointer_single_property) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
  // Different pointer should have different hash (likely)
  const sourcemeta::core::Pointer other{"bar"};
  EXPECT_NE(hash, hasher(other));
}

TEST(JSONPointer_hash, pointer_single_index) {
  const sourcemeta::core::Pointer pointer{0};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
  // Different index should have different hash
  const sourcemeta::core::Pointer other{1};
  EXPECT_NE(hash, hasher(other));
}

TEST(JSONPointer_hash, pointer_mixed_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 0, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, pointer_equality_consistency) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  // Equal pointers must have equal hashes
  EXPECT_EQ(pointer1, pointer2);
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_different_sizes) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar"};
  const sourcemeta::core::Pointer pointer3{"foo", "bar", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  // Different sizes should produce different hashes (likely)
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
  EXPECT_NE(hasher(pointer2), hasher(pointer3));
  EXPECT_NE(hasher(pointer1), hasher(pointer3));
}

TEST(JSONPointer_hash, pointer_sampling_first_last) {
  // Test that first and last tokens are sampled
  const sourcemeta::core::Pointer pointer1{"a", "x", "x", "x", "b"};
  const sourcemeta::core::Pointer pointer2{"a", "y", "y", "y", "b"};
  const sourcemeta::core::Pointer pointer3{"c", "x", "x", "x", "b"};
  const sourcemeta::core::Pointer pointer4{"a", "x", "x", "x", "d"};
  const std::hash<sourcemeta::core::Pointer> hasher;

  const auto hash1 = hasher(pointer1);
  const auto hash2 = hasher(pointer2);
  const auto hash3 = hasher(pointer3);
  const auto hash4 = hasher(pointer4);

  // Same first and last, different middle (at index 2) - should differ
  // because middle is sampled
  EXPECT_NE(hash1, hash2);

  // Different first, same last - should differ
  EXPECT_NE(hash1, hash3);

  // Same first, different last - should differ
  EXPECT_NE(hash1, hash4);
}

TEST(JSONPointer_hash, pointer_sampling_middle) {
  // Test that middle token is sampled for size >= 3
  const sourcemeta::core::Pointer pointer1{"a", "x", "b"};
  const sourcemeta::core::Pointer pointer2{"a", "y", "b"};
  const std::hash<sourcemeta::core::Pointer> hasher;

  // Different middle token should produce different hash
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer key1{"foo"};
  const sourcemeta::core::Pointer key2{"bar"};
  const sourcemeta::core::Pointer key3{"baz"};

  map[key1] = 1;
  map[key2] = 2;
  map[key3] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[key1], 1);
  EXPECT_EQ(map[key2], 2);
  EXPECT_EQ(map[key3], 3);

  // Test lookup with equivalent pointer
  const sourcemeta::core::Pointer lookup{"foo"};
  EXPECT_EQ(map[lookup], 1);
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const sourcemeta::core::Pointer pointer3{"foo"}; // Duplicate

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2); // Only 2 unique pointers
  EXPECT_TRUE(set.find(pointer1) != set.end());
  EXPECT_TRUE(set.find(pointer2) != set.end());
}

TEST(JSONPointer_hash, pointer_collision_resolution) {
  // Test that different pointers are stored correctly in unordered_map
  // Even if hashes were to collide, equality should resolve correctly
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer1{"a", "x", "x", "x", "b"};
  const sourcemeta::core::Pointer pointer2{"a", "y", "y", "y", "b"};

  map[pointer1] = 1;
  map[pointer2] = 2;

  // Both should be stored correctly (they have different hashes due to
  // different middle tokens, but this tests the map behavior)
  EXPECT_EQ(map[pointer1], 1);
  EXPECT_EQ(map[pointer2], 2);
}

TEST(JSONPointer_hash, pointer_long_sequence) {
  // Test with longer pointer to ensure O(1) behavior
  const sourcemeta::core::Pointer pointer{"a", "b", "c", "d", "e",
                                          "f", "g", "h", "i", "j"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, weakpointer_empty) {
  const sourcemeta::core::WeakPointer pointer;
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic for empty pointer
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, weakpointer_single_property) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, weakpointer_single_index) {
  const sourcemeta::core::WeakPointer pointer{0};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, weakpointer_mixed_tokens) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), 0,
                                              std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, weakpointer_equality_consistency) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar),
                                               std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar),
                                               std::cref(baz)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  // Equal pointers must have equal hashes
  EXPECT_EQ(pointer1, pointer2);
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_unordered_map) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer key1{std::cref(foo)};
  const sourcemeta::core::WeakPointer key2{std::cref(bar)};
  const sourcemeta::core::WeakPointer key3{std::cref(baz)};

  map[key1] = 1;
  map[key2] = 2;
  map[key3] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[key1], 1);
  EXPECT_EQ(map[key2], 2);
  EXPECT_EQ(map[key3], 3);
}

TEST(JSONPointer_hash, weakpointer_unordered_set) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo)}; // Duplicate

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2); // Only 2 unique pointers
  EXPECT_TRUE(set.find(pointer1) != set.end());
  EXPECT_TRUE(set.find(pointer2) != set.end());
}

TEST(JSONPointer_hash, pointer_weakpointer_compatible_hashes) {
  // Test that Pointer and WeakPointer produce compatible hash distributions
  // for equivalent sequences (though they're distinct types)
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer weak_pointer{std::cref(foo),
                                                   std::cref(bar)};

  const std::hash<sourcemeta::core::Pointer> pointer_hasher;
  const std::hash<sourcemeta::core::WeakPointer> weak_hasher;

  // While not required to be identical, the hashing logic should be similar
  // This test just ensures both can be hashed without errors
  const auto hash1 = pointer_hasher(pointer);
  const auto hash2 = weak_hasher(weak_pointer);

  // Both should produce valid hashes
  EXPECT_NE(hash1, 0); // Unlikely to be exactly 0
  EXPECT_NE(hash2, 0); // Unlikely to be exactly 0
}

TEST(JSONPointer_hash, pointer_with_indices) {
  const sourcemeta::core::Pointer pointer1{0, 1, 2};
  const sourcemeta::core::Pointer pointer2{0, 1, 3};
  const std::hash<sourcemeta::core::Pointer> hasher;

  // Different indices should produce different hashes
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_property_vs_index) {
  // Test that property "0" and index 0 produce different hashes
  const sourcemeta::core::Pointer pointer_property{"0"};
  const sourcemeta::core::Pointer pointer_index{0};
  const std::hash<sourcemeta::core::Pointer> hasher;

  // These should be different (property vs index)
  EXPECT_NE(pointer_property, pointer_index);
  EXPECT_NE(hasher(pointer_property), hasher(pointer_index));
}
