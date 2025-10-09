#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

static const std::string foo = "foo";
static const std::string bar = "bar";
static const std::string baz = "baz";

TEST(JSONPointer_hash, pointer_empty) {
  const sourcemeta::core::Pointer pointer1;
  const sourcemeta::core::Pointer pointer2;
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_single_property) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"foo"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_single_index) {
  const sourcemeta::core::Pointer pointer1{0};
  const sourcemeta::core::Pointer pointer2{0};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_two_tokens) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_multiple_tokens) {
  const sourcemeta::core::Pointer pointer1{"foo", 1, "bar", 2, "baz"};
  const sourcemeta::core::Pointer pointer2{"foo", 1, "bar", 2, "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_different_pointers) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const sourcemeta::core::Pointer pointer3{"foo"};

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
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};

  map[pointer1] = 1;
  map[pointer2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 1);
  EXPECT_EQ(map[pointer2], 2);
}

TEST(JSONPointer_hash, pointer_consistency) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar", 2, "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1 = hasher(pointer);
  const auto hash2 = hasher(pointer);
  const auto hash3 = hasher(pointer);
  EXPECT_EQ(hash1, hash2);
  EXPECT_EQ(hash2, hash3);
}

TEST(JSONPointer_hash, weakpointer_empty) {
  const sourcemeta::core::WeakPointer pointer1;
  const sourcemeta::core::WeakPointer pointer2;
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_single_property) {
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_single_index) {
  const sourcemeta::core::WeakPointer pointer1{0};
  const sourcemeta::core::WeakPointer pointer2{0};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_multiple_tokens) {
  sourcemeta::core::WeakPointer pointer1;
  pointer1.push_back(std::cref(foo));
  pointer1.push_back(1);
  pointer1.push_back(std::cref(bar));
  pointer1.push_back(2);
  pointer1.push_back(std::cref(baz));

  sourcemeta::core::WeakPointer pointer2;
  pointer2.push_back(std::cref(foo));
  pointer2.push_back(1);
  pointer2.push_back(std::cref(bar));
  pointer2.push_back(2);
  pointer2.push_back(std::cref(baz));

  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_unordered_set) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo)};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(pointer1) != set.end());
  EXPECT_TRUE(set.find(pointer2) != set.end());
  EXPECT_TRUE(set.find(pointer3) != set.end());
}

TEST(JSONPointer_hash, weakpointer_unordered_map) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};

  map[pointer1] = 1;
  map[pointer2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 1);
  EXPECT_EQ(map[pointer2], 2);
}

TEST(JSONPointer_hash, weakpointer_consistency) {
  sourcemeta::core::WeakPointer pointer;
  pointer.push_back(std::cref(foo));
  pointer.push_back(1);
  pointer.push_back(std::cref(bar));
  pointer.push_back(2);
  pointer.push_back(std::cref(baz));

  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash1 = hasher(pointer);
  const auto hash2 = hasher(pointer);
  const auto hash3 = hasher(pointer);
  EXPECT_EQ(hash1, hash2);
  EXPECT_EQ(hash2, hash3);
}
