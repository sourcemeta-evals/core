#include <gtest/gtest.h>

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
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_equal_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer pointer_2{"foo", "bar", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_different_pointers_different_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  // Different pointers should typically have different hashes
  // (though collisions are possible)
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  const sourcemeta::core::Pointer pointer_1{"foo"};
  const sourcemeta::core::Pointer pointer_2{"bar"};
  const sourcemeta::core::Pointer pointer_3{"foo", "bar"};

  set.insert(pointer_1);
  set.insert(pointer_2);
  set.insert(pointer_3);

  EXPECT_EQ(set.size(), 3);
  EXPECT_TRUE(set.contains(pointer_1));
  EXPECT_TRUE(set.contains(pointer_2));
  EXPECT_TRUE(set.contains(pointer_3));

  // Insert duplicate
  set.insert(pointer_1);
  EXPECT_EQ(set.size(), 3);
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  const sourcemeta::core::Pointer pointer_1{"foo"};
  const sourcemeta::core::Pointer pointer_2{"bar"};
  const sourcemeta::core::Pointer pointer_3{0, "baz"};

  map[pointer_1] = 1;
  map[pointer_2] = 2;
  map[pointer_3] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[pointer_1], 1);
  EXPECT_EQ(map[pointer_2], 2);
  EXPECT_EQ(map[pointer_3], 3);

  // Update existing key
  map[pointer_1] = 10;
  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[pointer_1], 10);
}

TEST(JSONPointer_hash, pointer_find_in_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  set.insert(sourcemeta::core::Pointer{1, 2, 3});

  auto it = set.find(sourcemeta::core::Pointer{"foo", "bar"});
  EXPECT_NE(it, set.end());

  it = set.find(sourcemeta::core::Pointer{"not", "found"});
  EXPECT_EQ(it, set.end());
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

TEST(JSONPointer_hash, weakpointer_unordered_set) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;

  const std::string foo = "foo";
  const std::string bar = "bar";

  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_3{1};

  set.insert(pointer_1);
  set.insert(pointer_2);
  set.insert(pointer_3);

  EXPECT_EQ(set.size(), 3);
  EXPECT_TRUE(set.contains(pointer_1));
  EXPECT_TRUE(set.contains(pointer_2));
  EXPECT_TRUE(set.contains(pointer_3));
}

TEST(JSONPointer_hash, weakpointer_unordered_map) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;

  const std::string foo = "foo";
  const std::string bar = "bar";

  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(bar)};

  map[pointer_1] = 1;
  map[pointer_2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer_1], 1);
  EXPECT_EQ(map[pointer_2], 2);
}

TEST(JSONPointer_hash, pointer_and_weakpointer_same_content_same_hash) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};

  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer weak_pointer{std::cref(foo),
                                                   std::cref(bar)};

  const std::hash<sourcemeta::core::Pointer> pointer_hasher;
  const std::hash<sourcemeta::core::WeakPointer> weak_pointer_hasher;

  // Both should produce the same hash for the same logical content
  EXPECT_EQ(pointer_hasher(pointer), weak_pointer_hasher(weak_pointer));
}

TEST(JSONPointer_hash, pointer_long_path) {
  const sourcemeta::core::Pointer pointer{"a", "b", "c", "d", "e",
                                          "f", "g", "h", "i", "j"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_mixed_indices_and_properties) {
  const sourcemeta::core::Pointer pointer{0, "foo", 1, "bar", 2};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);

  // Test in unordered_set
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(pointer);
  EXPECT_TRUE(set.contains(pointer));
}

TEST(JSONPointer_hash, pointer_two_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}
