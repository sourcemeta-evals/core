#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <string>
#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, unordered_map_pointer_basic) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  const sourcemeta::core::Pointer p1{"foo", "bar"};
  const sourcemeta::core::Pointer p2{"baz"};
  const sourcemeta::core::Pointer p3{0, "test"};

  map[p1] = 1;
  map[p2] = 2;
  map[p3] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map.at(p1), 1);
  EXPECT_EQ(map.at(p2), 2);
  EXPECT_EQ(map.at(p3), 3);
}

TEST(JSONPointer_hash, unordered_map_pointer_find) {
  std::unordered_map<sourcemeta::core::Pointer, std::string> map;

  const sourcemeta::core::Pointer p1{"foo"};
  const sourcemeta::core::Pointer p2{"bar", 1};

  map[p1] = "first";
  map[p2] = "second";

  auto it1 = map.find(p1);
  EXPECT_NE(it1, map.end());
  EXPECT_EQ(it1->second, "first");

  auto it2 = map.find(p2);
  EXPECT_NE(it2, map.end());
  EXPECT_EQ(it2->second, "second");

  const sourcemeta::core::Pointer p3{"nonexistent"};
  auto it3 = map.find(p3);
  EXPECT_EQ(it3, map.end());
}

TEST(JSONPointer_hash, unordered_map_pointer_erase) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  const sourcemeta::core::Pointer p1{"foo"};
  const sourcemeta::core::Pointer p2{"bar"};

  map[p1] = 1;
  map[p2] = 2;

  EXPECT_EQ(map.size(), 2);

  map.erase(p1);
  EXPECT_EQ(map.size(), 1);
  EXPECT_EQ(map.count(p1), 0);
  EXPECT_EQ(map.count(p2), 1);
}

TEST(JSONPointer_hash, unordered_set_pointer_basic) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  const sourcemeta::core::Pointer p1{"foo", "bar"};
  const sourcemeta::core::Pointer p2{"baz"};
  const sourcemeta::core::Pointer p3{0, 1, "test"};

  set.insert(p1);
  set.insert(p2);
  set.insert(p3);

  EXPECT_EQ(set.size(), 3);
  EXPECT_EQ(set.count(p1), 1);
  EXPECT_EQ(set.count(p2), 1);
  EXPECT_EQ(set.count(p3), 1);
}

TEST(JSONPointer_hash, unordered_set_pointer_duplicates) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  const sourcemeta::core::Pointer p1{"foo", "bar"};

  set.insert(p1);
  set.insert(p1);

  EXPECT_EQ(set.size(), 1);
}

TEST(JSONPointer_hash, unordered_map_weakpointer_basic) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  const sourcemeta::core::WeakPointer p1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer p2{std::cref(baz)};

  map[p1] = 1;
  map[p2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(p1), 1);
  EXPECT_EQ(map.at(p2), 2);
}

TEST(JSONPointer_hash, unordered_set_weakpointer_basic) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  const sourcemeta::core::WeakPointer p1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer p2{std::cref(baz)};

  set.insert(p1);
  set.insert(p2);

  EXPECT_EQ(set.size(), 2);
  EXPECT_EQ(set.count(p1), 1);
  EXPECT_EQ(set.count(p2), 1);
}

TEST(JSONPointer_hash, hash_consistency_pointer) {
  const sourcemeta::core::Pointer p1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer p2{"foo", "bar", "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
}

TEST(JSONPointer_hash, hash_consistency_weakpointer) {
  const std::string foo = "foo";
  const std::string bar = "bar";

  const sourcemeta::core::WeakPointer p1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer p2{std::cref(foo), std::cref(bar)};

  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
}

TEST(JSONPointer_hash, empty_pointer) {
  const sourcemeta::core::Pointer p1;
  const sourcemeta::core::Pointer p2;

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), 0);
  EXPECT_EQ(hasher(p1), hasher(p2));
}

TEST(JSONPointer_hash, single_token_property) {
  const sourcemeta::core::Pointer p{"foo"};

  std::hash<sourcemeta::core::Pointer> hasher;
  auto hash_value = hasher(p);

  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(p);
  EXPECT_EQ(set.count(p), 1);
}

TEST(JSONPointer_hash, single_token_index) {
  const sourcemeta::core::Pointer p{42};

  std::hash<sourcemeta::core::Pointer> hasher;
  auto hash_value = hasher(p);

  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(p);
  EXPECT_EQ(set.count(p), 1);
}

TEST(JSONPointer_hash, mixed_tokens) {
  const sourcemeta::core::Pointer p1{"foo", 0, "bar", 1, "baz"};
  const sourcemeta::core::Pointer p2{"foo", 0, "bar", 1, "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));

  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map[p1] = 123;
  EXPECT_EQ(map.at(p2), 123);
}
