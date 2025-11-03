#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_in_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;

  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"baz", "qux"};
  const sourcemeta::core::Pointer pointer3{"foo", "bar"};

  pointers.insert(pointer1);
  pointers.insert(pointer2);
  pointers.insert(pointer3);

  EXPECT_EQ(pointers.size(), 2);
  EXPECT_TRUE(pointers.find(pointer1) != pointers.end());
  EXPECT_TRUE(pointers.find(pointer2) != pointers.end());
  EXPECT_TRUE(pointers.find(pointer3) != pointers.end());
}

TEST(JSONPointer_hash, pointer_in_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> pointer_map;

  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};

  pointer_map[pointer1] = 1;
  pointer_map[pointer2] = 2;

  EXPECT_EQ(pointer_map.size(), 2);
  EXPECT_EQ(pointer_map[pointer1], 1);
  EXPECT_EQ(pointer_map[pointer2], 2);
}

TEST(JSONPointer_hash, weak_pointer_in_unordered_set) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const std::string qux = "qux";

  std::unordered_set<sourcemeta::core::WeakPointer> pointers;

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz), std::cref(qux)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo), std::cref(bar)};

  pointers.insert(pointer1);
  pointers.insert(pointer2);
  pointers.insert(pointer3);

  EXPECT_EQ(pointers.size(), 2);
  EXPECT_TRUE(pointers.find(pointer1) != pointers.end());
  EXPECT_TRUE(pointers.find(pointer2) != pointers.end());
  EXPECT_TRUE(pointers.find(pointer3) != pointers.end());
}

TEST(JSONPointer_hash, weak_pointer_in_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";

  std::unordered_map<sourcemeta::core::WeakPointer, int> pointer_map;

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};

  pointer_map[pointer1] = 1;
  pointer_map[pointer2] = 2;

  EXPECT_EQ(pointer_map.size(), 2);
  EXPECT_EQ(pointer_map[pointer1], 1);
  EXPECT_EQ(pointer_map[pointer2], 2);
}

TEST(JSONPointer_hash, empty_pointer) {
  const sourcemeta::core::Pointer empty1;
  const sourcemeta::core::Pointer empty2;

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(empty1), hasher(empty2));
  EXPECT_EQ(hasher(empty1), 0);
}

TEST(JSONPointer_hash, single_token_property) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"foo"};
  const sourcemeta::core::Pointer pointer3{"bar"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
  EXPECT_NE(hasher(pointer1), hasher(pointer3));
}

TEST(JSONPointer_hash, single_token_index) {
  const sourcemeta::core::Pointer pointer1{0};
  const sourcemeta::core::Pointer pointer2{0};
  const sourcemeta::core::Pointer pointer3{1};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
  EXPECT_NE(hasher(pointer1), hasher(pointer3));
}

TEST(JSONPointer_hash, multiple_tokens) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer pointer3{"foo", "qux", "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
  EXPECT_NE(hasher(pointer1), hasher(pointer3));
}

TEST(JSONPointer_hash, mixed_tokens) {
  const sourcemeta::core::Pointer pointer1{"foo", 0, "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", 0, "bar"};
  const sourcemeta::core::Pointer pointer3{"foo", 1, "bar"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
  EXPECT_NE(hasher(pointer1), hasher(pointer3));
}

TEST(JSONPointer_hash, consistency) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", 0, "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1 = hasher(pointer);
  const auto hash2 = hasher(pointer);
  const auto hash3 = hasher(pointer);

  EXPECT_EQ(hash1, hash2);
  EXPECT_EQ(hash2, hash3);
}
