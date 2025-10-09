#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_unordered_set_basic) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  const sourcemeta::core::Pointer p1{"foo"};
  const sourcemeta::core::Pointer p2{"bar"};
  const sourcemeta::core::Pointer p3{"foo"};

  set.insert(p1);
  set.insert(p2);
  set.insert(p3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(p1));
  EXPECT_TRUE(set.contains(p2));
  EXPECT_TRUE(set.contains(p3));
}

TEST(JSONPointer_hash, pointer_unordered_set_mixed) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  const sourcemeta::core::Pointer p1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer p2{"foo", 2, "bar"};
  const sourcemeta::core::Pointer p3{"foo", 1, "bar"};

  set.insert(p1);
  set.insert(p2);
  set.insert(p3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(p1));
  EXPECT_TRUE(set.contains(p2));
  EXPECT_FALSE(set.contains(sourcemeta::core::Pointer{"baz"}));
}

TEST(JSONPointer_hash, pointer_unordered_map_basic) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  const sourcemeta::core::Pointer p1{"foo"};
  const sourcemeta::core::Pointer p2{"bar"};

  map[p1] = 1;
  map[p2] = 2;
  map[p1] = 3;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(p1), 3);
  EXPECT_EQ(map.at(p2), 2);
}

TEST(JSONPointer_hash, pointer_unordered_map_complex) {
  std::unordered_map<sourcemeta::core::Pointer, std::string> map;

  const sourcemeta::core::Pointer p1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer p2{0, "item", 5};
  const sourcemeta::core::Pointer p3{"test"};

  map[p1] = "first";
  map[p2] = "second";
  map[p3] = "third";

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map.at(p1), "first");
  EXPECT_EQ(map.at(p2), "second");
  EXPECT_EQ(map.at(p3), "third");
}

TEST(JSONPointer_hash, pointer_empty) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  const sourcemeta::core::Pointer empty1;
  const sourcemeta::core::Pointer empty2;

  set.insert(empty1);
  set.insert(empty2);

  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{}));
}

TEST(JSONPointer_hash, weakpointer_unordered_set_basic) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;

  const std::string foo = "foo";
  const std::string bar = "bar";

  const sourcemeta::core::WeakPointer p1{std::cref(foo)};
  const sourcemeta::core::WeakPointer p2{std::cref(bar)};
  const sourcemeta::core::WeakPointer p3{std::cref(foo)};

  set.insert(p1);
  set.insert(p2);
  set.insert(p3);

  EXPECT_EQ(set.size(), 2);
}

TEST(JSONPointer_hash, weakpointer_unordered_map_basic) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;

  const std::string foo = "foo";
  const std::string bar = "bar";

  const sourcemeta::core::WeakPointer p1{std::cref(foo)};
  const sourcemeta::core::WeakPointer p2{std::cref(bar)};

  map[p1] = 1;
  map[p2] = 2;
  map[p1] = 3;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(p1), 3);
  EXPECT_EQ(map.at(p2), 2);
}

TEST(JSONPointer_hash, hash_different_for_different_pointers) {
  const sourcemeta::core::Pointer p1{"foo"};
  const sourcemeta::core::Pointer p2{"bar"};
  const sourcemeta::core::Pointer p3{"foo", "bar"};

  std::hash<sourcemeta::core::Pointer> hasher;

  const auto h1 = hasher(p1);
  const auto h2 = hasher(p2);
  const auto h3 = hasher(p3);

  EXPECT_NE(h1, h2);
  EXPECT_NE(h1, h3);
  EXPECT_NE(h2, h3);
}

TEST(JSONPointer_hash, hash_same_for_equal_pointers) {
  const sourcemeta::core::Pointer p1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer p2{"foo", 1, "bar"};

  std::hash<sourcemeta::core::Pointer> hasher;

  EXPECT_EQ(hasher(p1), hasher(p2));
}
