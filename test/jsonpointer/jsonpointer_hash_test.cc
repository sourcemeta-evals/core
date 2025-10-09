#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <functional>
#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_in_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;

  const sourcemeta::core::Pointer p1{"foo", "bar"};
  const sourcemeta::core::Pointer p2{"foo", "baz"};
  const sourcemeta::core::Pointer p3{"foo", "bar"};

  pointers.insert(p1);
  pointers.insert(p2);
  pointers.insert(p3);

  EXPECT_EQ(pointers.size(), 2);
  EXPECT_TRUE(pointers.count(p1) > 0);
  EXPECT_TRUE(pointers.count(p2) > 0);
  EXPECT_TRUE(pointers.count(p3) > 0);
}

TEST(JSONPointer_hash, pointer_in_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> pointer_map;

  const sourcemeta::core::Pointer p1{"foo"};
  const sourcemeta::core::Pointer p2{"bar"};
  const sourcemeta::core::Pointer p3{"foo"};

  pointer_map[p1] = 1;
  pointer_map[p2] = 2;
  pointer_map[p3] = 3;

  EXPECT_EQ(pointer_map.size(), 2);
  EXPECT_EQ(pointer_map[p1], 3);
  EXPECT_EQ(pointer_map[p2], 2);
}

TEST(JSONPointer_hash, weakpointer_in_unordered_set) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  std::unordered_set<sourcemeta::core::WeakPointer> pointers;

  const sourcemeta::core::WeakPointer p1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer p2{std::cref(foo), std::cref(baz)};
  const sourcemeta::core::WeakPointer p3{std::cref(foo), std::cref(bar)};

  pointers.insert(p1);
  pointers.insert(p2);
  pointers.insert(p3);

  EXPECT_EQ(pointers.size(), 2);
  EXPECT_TRUE(pointers.count(p1) > 0);
  EXPECT_TRUE(pointers.count(p2) > 0);
  EXPECT_TRUE(pointers.count(p3) > 0);
}

TEST(JSONPointer_hash, weakpointer_in_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";

  std::unordered_map<sourcemeta::core::WeakPointer, int> pointer_map;

  const sourcemeta::core::WeakPointer p1{std::cref(foo)};
  const sourcemeta::core::WeakPointer p2{std::cref(bar)};
  const sourcemeta::core::WeakPointer p3{std::cref(foo)};

  pointer_map[p1] = 1;
  pointer_map[p2] = 2;
  pointer_map[p3] = 3;

  EXPECT_EQ(pointer_map.size(), 2);
  EXPECT_EQ(pointer_map[p1], 3);
  EXPECT_EQ(pointer_map[p2], 2);
}

TEST(JSONPointer_hash, equal_pointers_equal_hashes) {
  const sourcemeta::core::Pointer p1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer p2{"foo", "bar", "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
}

TEST(JSONPointer_hash, different_pointers_different_hashes) {
  const sourcemeta::core::Pointer p1{"foo", "bar"};
  const sourcemeta::core::Pointer p2{"foo", "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(p1), hasher(p2));
}

TEST(JSONPointer_hash, empty_pointer) {
  const sourcemeta::core::Pointer p1;
  const sourcemeta::core::Pointer p2;

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
  EXPECT_EQ(hasher(p1), 0);
}

TEST(JSONPointer_hash, index_tokens) {
  const sourcemeta::core::Pointer p1{0, 1, 2};
  const sourcemeta::core::Pointer p2{0, 1, 2};
  const sourcemeta::core::Pointer p3{0, 1, 3};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
  EXPECT_NE(hasher(p1), hasher(p3));
}

TEST(JSONPointer_hash, mixed_tokens) {
  const sourcemeta::core::Pointer p1{"foo", 0, "bar"};
  const sourcemeta::core::Pointer p2{"foo", 0, "bar"};
  const sourcemeta::core::Pointer p3{"foo", 1, "bar"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
  EXPECT_NE(hasher(p1), hasher(p3));
}

TEST(JSONPointer_hash, single_token_property) {
  const sourcemeta::core::Pointer p1{"foo"};
  const sourcemeta::core::Pointer p2{"foo"};
  const sourcemeta::core::Pointer p3{"bar"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
  EXPECT_NE(hasher(p1), hasher(p3));
}

TEST(JSONPointer_hash, single_token_index) {
  const sourcemeta::core::Pointer p1{5};
  const sourcemeta::core::Pointer p2{5};
  const sourcemeta::core::Pointer p3{10};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
  EXPECT_NE(hasher(p1), hasher(p3));
}

TEST(JSONPointer_hash, two_tokens) {
  const sourcemeta::core::Pointer p1{"foo", "bar"};
  const sourcemeta::core::Pointer p2{"foo", "bar"};
  const sourcemeta::core::Pointer p3{"foo", "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
  EXPECT_NE(hasher(p1), hasher(p3));
}

TEST(JSONPointer_hash, many_tokens) {
  const sourcemeta::core::Pointer p1{"a", "b", "c", "d", "e", "f"};
  const sourcemeta::core::Pointer p2{"a", "b", "c", "d", "e", "f"};
  const sourcemeta::core::Pointer p3{"a", "b", "c", "x", "e", "f"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
  EXPECT_NE(hasher(p1), hasher(p3));
}
