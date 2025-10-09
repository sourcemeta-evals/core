#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <functional>
#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_unordered_set_basic) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  sourcemeta::core::Pointer p1{"foo"};
  sourcemeta::core::Pointer p2{"bar"};
  sourcemeta::core::Pointer p3{"foo"};

  set.insert(p1);
  set.insert(p2);
  set.insert(p3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.count(p1) > 0);
  EXPECT_TRUE(set.count(p2) > 0);
  EXPECT_TRUE(set.count(p3) > 0);
}

TEST(JSONPointer_hash, pointer_unordered_set_empty) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  sourcemeta::core::Pointer empty;
  set.insert(empty);

  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.count(empty) > 0);
}

TEST(JSONPointer_hash, pointer_unordered_set_mixed_tokens) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  sourcemeta::core::Pointer p1{"foo", 1, "bar"};
  sourcemeta::core::Pointer p2{"foo", 2, "bar"};
  sourcemeta::core::Pointer p3{"foo", 1, "bar"};

  set.insert(p1);
  set.insert(p2);
  set.insert(p3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.count(p1) > 0);
  EXPECT_TRUE(set.count(p2) > 0);
}

TEST(JSONPointer_hash, pointer_unordered_map_basic) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  sourcemeta::core::Pointer p1{"foo"};
  sourcemeta::core::Pointer p2{"bar"};

  map[p1] = 1;
  map[p2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[p1], 1);
  EXPECT_EQ(map[p2], 2);
}

TEST(JSONPointer_hash, pointer_unordered_map_update_value) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  sourcemeta::core::Pointer p1{"foo", "bar"};

  map[p1] = 1;
  EXPECT_EQ(map[p1], 1);

  map[p1] = 2;
  EXPECT_EQ(map[p1], 2);
  EXPECT_EQ(map.size(), 1);
}

TEST(JSONPointer_hash, pointer_unordered_map_long_path) {
  std::unordered_map<sourcemeta::core::Pointer, std::string> map;

  sourcemeta::core::Pointer p1{"foo", "bar", "baz", "qux", "quux"};
  sourcemeta::core::Pointer p2{"foo", "bar", "baz", "qux", "quuz"};

  map[p1] = "first";
  map[p2] = "second";

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[p1], "first");
  EXPECT_EQ(map[p2], "second");
}

TEST(JSONPointer_hash, weakpointer_unordered_set_basic) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;

  const std::string foo = "foo";
  const std::string bar = "bar";

  sourcemeta::core::WeakPointer p1{std::cref(foo)};
  sourcemeta::core::WeakPointer p2{std::cref(bar)};
  sourcemeta::core::WeakPointer p3{std::cref(foo)};

  set.insert(p1);
  set.insert(p2);
  set.insert(p3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.count(p1) > 0);
  EXPECT_TRUE(set.count(p2) > 0);
}

TEST(JSONPointer_hash, weakpointer_unordered_set_empty) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;

  sourcemeta::core::WeakPointer empty;
  set.insert(empty);

  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.count(empty) > 0);
}

TEST(JSONPointer_hash, weakpointer_unordered_set_mixed_tokens) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;

  const std::string foo = "foo";
  const std::string bar = "bar";

  sourcemeta::core::WeakPointer p1{std::cref(foo), 1, std::cref(bar)};
  sourcemeta::core::WeakPointer p2{std::cref(foo), 2, std::cref(bar)};
  sourcemeta::core::WeakPointer p3{std::cref(foo), 1, std::cref(bar)};

  set.insert(p1);
  set.insert(p2);
  set.insert(p3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.count(p1) > 0);
  EXPECT_TRUE(set.count(p2) > 0);
}

TEST(JSONPointer_hash, weakpointer_unordered_map_basic) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;

  const std::string foo = "foo";
  const std::string bar = "bar";

  sourcemeta::core::WeakPointer p1{std::cref(foo)};
  sourcemeta::core::WeakPointer p2{std::cref(bar)};

  map[p1] = 1;
  map[p2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[p1], 1);
  EXPECT_EQ(map[p2], 2);
}

TEST(JSONPointer_hash, weakpointer_unordered_map_update_value) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;

  const std::string foo = "foo";
  const std::string bar = "bar";

  sourcemeta::core::WeakPointer p1{std::cref(foo), std::cref(bar)};

  map[p1] = 1;
  EXPECT_EQ(map[p1], 1);

  map[p1] = 2;
  EXPECT_EQ(map[p1], 2);
  EXPECT_EQ(map.size(), 1);
}

TEST(JSONPointer_hash, pointer_hash_consistency) {
  sourcemeta::core::Pointer p1{"foo", "bar", "baz"};
  sourcemeta::core::Pointer p2{"foo", "bar", "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
}

TEST(JSONPointer_hash, weakpointer_hash_consistency) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  sourcemeta::core::WeakPointer p1{std::cref(foo), std::cref(bar),
                                   std::cref(baz)};
  sourcemeta::core::WeakPointer p2{std::cref(foo), std::cref(bar),
                                   std::cref(baz)};

  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(p1), hasher(p2));
}

TEST(JSONPointer_hash, pointer_with_indices) {
  std::unordered_map<sourcemeta::core::Pointer, std::string> map;

  sourcemeta::core::Pointer p1{0};
  sourcemeta::core::Pointer p2{1};
  sourcemeta::core::Pointer p3{0, 1};

  map[p1] = "first";
  map[p2] = "second";
  map[p3] = "third";

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[p1], "first");
  EXPECT_EQ(map[p2], "second");
  EXPECT_EQ(map[p3], "third");
}

TEST(JSONPointer_hash, weakpointer_with_indices) {
  std::unordered_map<sourcemeta::core::WeakPointer, std::string> map;

  sourcemeta::core::WeakPointer p1{0};
  sourcemeta::core::WeakPointer p2{1};
  sourcemeta::core::WeakPointer p3{0, 1};

  map[p1] = "first";
  map[p2] = "second";
  map[p3] = "third";

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[p1], "first");
  EXPECT_EQ(map[p2], "second");
  EXPECT_EQ(map[p3], "third");
}
