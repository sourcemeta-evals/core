#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_empty_hash) {
  const sourcemeta::core::Pointer pointer1;
  const sourcemeta::core::Pointer pointer2;
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_same_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_different_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer pointer2{"foo", "qux", "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_with_indices) {
  const sourcemeta::core::Pointer pointer1{"foo", 0, "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", 0, "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_unordered_set) {
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

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"baz", "qux"};

  map[pointer1] = 42;
  map[pointer2] = 100;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 42);
  EXPECT_EQ(map[pointer2], 100);
}

TEST(JSONPointer_hash, pointer_single_token) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"foo"};
  const sourcemeta::core::Pointer pointer3{"bar"};
  std::hash<sourcemeta::core::Pointer> hasher;

  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
  EXPECT_NE(hasher(pointer1), hasher(pointer3));
}

TEST(JSONPointer_hash, weakpointer_empty_hash) {
  const sourcemeta::core::WeakPointer pointer1;
  const sourcemeta::core::WeakPointer pointer2;
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_same_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar),
                                               std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar),
                                               std::cref(baz)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_different_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const std::string qux = "qux";

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar),
                                               std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(qux),
                                               std::cref(baz)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_with_indices) {
  const std::string foo = "foo";
  const std::string bar = "bar";

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), 0,
                                               std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), 0,
                                               std::cref(bar)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_unordered_set) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const std::string qux = "qux";

  std::unordered_set<sourcemeta::core::WeakPointer> set;

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

TEST(JSONPointer_hash, weakpointer_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const std::string qux = "qux";

  std::unordered_map<sourcemeta::core::WeakPointer, int> map;

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz), std::cref(qux)};

  map[pointer1] = 42;
  map[pointer2] = 100;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 42);
  EXPECT_EQ(map[pointer2], 100);
}
