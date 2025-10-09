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
  set.insert(pointer1);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(pointer1) != set.end());
  EXPECT_TRUE(set.find(pointer2) != set.end());
}

TEST(JSONPointer_hash, pointer_in_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar", 1};

  map[pointer1] = 42;
  map[pointer2] = 99;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(pointer1), 42);
  EXPECT_EQ(map.at(pointer2), 99);
}

TEST(JSONPointer_hash, pointer_hash_consistency) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1 = hasher(pointer);
  const auto hash2 = hasher(pointer);

  EXPECT_EQ(hash1, hash2);
}

TEST(JSONPointer_hash, pointer_empty_hash) {
  const sourcemeta::core::Pointer pointer1;
  const sourcemeta::core::Pointer pointer2;

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
  EXPECT_EQ(hasher(pointer1), 0);
}

TEST(JSONPointer_hash, pointer_single_token) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{42};

  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1 = hasher(pointer1);
  const auto hash2 = hasher(pointer2);

  EXPECT_NE(hash1, 0);
  EXPECT_NE(hash2, 0);
}

TEST(JSONPointer_hash, pointer_multiple_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar", 2, "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_result = hasher(pointer);

  EXPECT_NE(hash_result, 0);
}

TEST(JSONPointer_hash, weakpointer_in_unordered_set) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  std::unordered_set<sourcemeta::core::WeakPointer> set;

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz)};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer1);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(pointer1) != set.end());
  EXPECT_TRUE(set.find(pointer2) != set.end());
}

TEST(JSONPointer_hash, weakpointer_in_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";

  std::unordered_map<sourcemeta::core::WeakPointer, int> map;

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar), 1};

  map[pointer1] = 42;
  map[pointer2] = 99;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(pointer1), 42);
  EXPECT_EQ(map.at(pointer2), 99);
}

TEST(JSONPointer_hash, weakpointer_hash_consistency) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};

  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash1 = hasher(pointer);
  const auto hash2 = hasher(pointer);

  EXPECT_EQ(hash1, hash2);
}

TEST(JSONPointer_hash, weakpointer_empty_hash) {
  const sourcemeta::core::WeakPointer pointer1;
  const sourcemeta::core::WeakPointer pointer2;

  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
  EXPECT_EQ(hasher(pointer1), 0);
}

TEST(JSONPointer_hash, weakpointer_single_token) {
  const std::string foo = "foo";

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{42};

  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash1 = hasher(pointer1);
  const auto hash2 = hasher(pointer2);

  EXPECT_NE(hash1, 0);
  EXPECT_NE(hash2, 0);
}

TEST(JSONPointer_hash, weakpointer_multiple_tokens) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  const sourcemeta::core::WeakPointer pointer{std::cref(foo), 1, std::cref(bar),
                                              2, std::cref(baz)};

  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_result = hasher(pointer);

  EXPECT_NE(hash_result, 0);
}
