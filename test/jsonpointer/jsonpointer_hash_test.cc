#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, unordered_map_pointer_basic) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  const sourcemeta::core::Pointer p1{"foo", "bar"};
  const sourcemeta::core::Pointer p2{"baz"};

  map[p1] = 1;
  map[p2] = 2;

  EXPECT_EQ(map[p1], 1);
  EXPECT_EQ(map[p2], 2);
  EXPECT_EQ(map.size(), 2);
}

TEST(JSONPointer_hash, unordered_set_pointer_basic) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  const sourcemeta::core::Pointer p1{"foo", "bar"};
  const sourcemeta::core::Pointer p2{"baz"};

  set.insert(p1);
  set.insert(p2);
  set.insert(p1);

  EXPECT_EQ(set.size(), 2);
  EXPECT_EQ(set.count(p1), 1);
  EXPECT_EQ(set.count(p2), 1);
}

TEST(JSONPointer_hash, unordered_map_weakpointer_basic) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;

  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};

  const sourcemeta::core::WeakPointer p1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer p2{std::cref(baz)};

  map[p1] = 1;
  map[p2] = 2;

  EXPECT_EQ(map[p1], 1);
  EXPECT_EQ(map[p2], 2);
  EXPECT_EQ(map.size(), 2);
}

TEST(JSONPointer_hash, unordered_set_weakpointer_basic) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;

  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};

  const sourcemeta::core::WeakPointer p1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer p2{std::cref(baz)};

  set.insert(p1);
  set.insert(p2);
  set.insert(p1);

  EXPECT_EQ(set.size(), 2);
  EXPECT_EQ(set.count(p1), 1);
  EXPECT_EQ(set.count(p2), 1);
}

TEST(JSONPointer_hash, equal_pointers_same_hash) {
  const sourcemeta::core::Pointer p1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer p2{"foo", "bar", "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
}

TEST(JSONPointer_hash, empty_pointer_hash) {
  const sourcemeta::core::Pointer p1;
  const sourcemeta::core::Pointer p2;

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
  EXPECT_EQ(hasher(p1), 0);
}

TEST(JSONPointer_hash, single_token_property) {
  const sourcemeta::core::Pointer p1{"foo"};
  const sourcemeta::core::Pointer p2{"foo"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
}

TEST(JSONPointer_hash, single_token_index) {
  const sourcemeta::core::Pointer p1{0};
  const sourcemeta::core::Pointer p2{0};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
}

TEST(JSONPointer_hash, two_tokens) {
  const sourcemeta::core::Pointer p1{"foo", "bar"};
  const sourcemeta::core::Pointer p2{"foo", "bar"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
}

TEST(JSONPointer_hash, three_or_more_tokens) {
  const sourcemeta::core::Pointer p1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer p2{"foo", "bar", "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
}

TEST(JSONPointer_hash, mixed_property_and_index_tokens) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  const sourcemeta::core::Pointer p1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer p2{0, "baz"};
  const sourcemeta::core::Pointer p3{1, 2, 3};

  map[p1] = 1;
  map[p2] = 2;
  map[p3] = 3;

  EXPECT_EQ(map[p1], 1);
  EXPECT_EQ(map[p2], 2);
  EXPECT_EQ(map[p3], 3);
  EXPECT_EQ(map.size(), 3);
}
