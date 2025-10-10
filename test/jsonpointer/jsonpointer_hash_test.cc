#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_in_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"baz"};

  set.insert(pointer1);
  set.insert(pointer2);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.count(pointer1) > 0);
  EXPECT_TRUE(set.count(pointer2) > 0);
}

TEST(JSONPointer_hash, pointer_in_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"baz"};

  map[pointer1] = 42;
  map[pointer2] = 24;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(pointer1), 42);
  EXPECT_EQ(map.at(pointer2), 24);
}

TEST(JSONPointer_hash, equal_pointers_have_equal_hashes) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", 1};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", 1};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, empty_pointer_hash) {
  const sourcemeta::core::Pointer pointer1;
  const sourcemeta::core::Pointer pointer2;

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
  EXPECT_EQ(hasher(pointer1), 0);
}

TEST(JSONPointer_hash, single_token_property) {
  const sourcemeta::core::Pointer pointer{"foo"};
  std::hash<sourcemeta::core::Pointer> hasher;

  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, single_token_index) {
  const sourcemeta::core::Pointer pointer{42};
  std::hash<sourcemeta::core::Pointer> hasher;

  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, multiple_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar", 2, "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;

  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, different_pointers_different_hashes) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_in_unordered_set) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz)};

  set.insert(pointer1);
  set.insert(pointer2);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.count(pointer1) > 0);
  EXPECT_TRUE(set.count(pointer2) > 0);
}

TEST(JSONPointer_hash, weakpointer_in_unordered_map) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz)};

  map[pointer1] = 42;
  map[pointer2] = 24;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(pointer1), 42);
  EXPECT_EQ(map.at(pointer2), 24);
}

TEST(JSONPointer_hash, weakpointer_equal_hashes) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar),
                                               1};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar),
                                               1};

  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_empty) {
  const sourcemeta::core::WeakPointer pointer1;
  const sourcemeta::core::WeakPointer pointer2;

  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
  EXPECT_EQ(hasher(pointer1), 0);
}

TEST(JSONPointer_hash, weakpointer_different_hashes) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};

  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}
