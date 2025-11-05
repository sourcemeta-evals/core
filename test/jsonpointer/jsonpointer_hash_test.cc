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
  const sourcemeta::core::Pointer pointer3{"baz"};

  pointer_map[pointer1] = 1;
  pointer_map[pointer2] = 2;
  pointer_map[pointer3] = 3;

  EXPECT_EQ(pointer_map.size(), 3);
  EXPECT_EQ(pointer_map[pointer1], 1);
  EXPECT_EQ(pointer_map[pointer2], 2);
  EXPECT_EQ(pointer_map[pointer3], 3);
}

TEST(JSONPointer_hash, pointer_with_indices) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;

  const sourcemeta::core::Pointer pointer1{0, 1, 2};
  const sourcemeta::core::Pointer pointer2{0, 1, 3};
  const sourcemeta::core::Pointer pointer3{0, 1, 2};

  pointers.insert(pointer1);
  pointers.insert(pointer2);
  pointers.insert(pointer3);

  EXPECT_EQ(pointers.size(), 2);
  EXPECT_TRUE(pointers.find(pointer1) != pointers.end());
  EXPECT_TRUE(pointers.find(pointer2) != pointers.end());
}

TEST(JSONPointer_hash, pointer_mixed_tokens) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;

  const sourcemeta::core::Pointer pointer1{"foo", 0, "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer3{"foo", 0, "bar"};

  pointers.insert(pointer1);
  pointers.insert(pointer2);
  pointers.insert(pointer3);

  EXPECT_EQ(pointers.size(), 2);
  EXPECT_TRUE(pointers.find(pointer1) != pointers.end());
  EXPECT_TRUE(pointers.find(pointer2) != pointers.end());
}

TEST(JSONPointer_hash, empty_pointer) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;

  const sourcemeta::core::Pointer empty1;
  const sourcemeta::core::Pointer empty2;

  pointers.insert(empty1);
  pointers.insert(empty2);

  EXPECT_EQ(pointers.size(), 1);
  EXPECT_TRUE(pointers.find(empty1) != pointers.end());
}

TEST(JSONPointer_hash, pointer_hash_consistency) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1 = hasher(pointer);
  const auto hash2 = hasher(pointer);

  EXPECT_EQ(hash1, hash2);
}

TEST(JSONPointer_hash, different_pointers_different_hashes) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};

  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1 = hasher(pointer1);
  const auto hash2 = hasher(pointer2);

  EXPECT_NE(hash1, hash2);
}

TEST(JSONPointer_hash, weakpointer_in_unordered_set) {
  std::unordered_set<sourcemeta::core::WeakPointer> pointers;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const std::string qux = "qux";

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

TEST(JSONPointer_hash, weakpointer_in_unordered_map) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> pointer_map;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(baz)};

  pointer_map[pointer1] = 1;
  pointer_map[pointer2] = 2;
  pointer_map[pointer3] = 3;

  EXPECT_EQ(pointer_map.size(), 3);
  EXPECT_EQ(pointer_map[pointer1], 1);
  EXPECT_EQ(pointer_map[pointer2], 2);
  EXPECT_EQ(pointer_map[pointer3], 3);
}

TEST(JSONPointer_hash, weakpointer_with_indices) {
  std::unordered_set<sourcemeta::core::WeakPointer> pointers;

  const sourcemeta::core::WeakPointer pointer1{0, 1, 2};
  const sourcemeta::core::WeakPointer pointer2{0, 1, 3};
  const sourcemeta::core::WeakPointer pointer3{0, 1, 2};

  pointers.insert(pointer1);
  pointers.insert(pointer2);
  pointers.insert(pointer3);

  EXPECT_EQ(pointers.size(), 2);
  EXPECT_TRUE(pointers.find(pointer1) != pointers.end());
  EXPECT_TRUE(pointers.find(pointer2) != pointers.end());
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

TEST(JSONPointer_hash, empty_weakpointer) {
  std::unordered_set<sourcemeta::core::WeakPointer> pointers;

  const sourcemeta::core::WeakPointer empty1;
  const sourcemeta::core::WeakPointer empty2;

  pointers.insert(empty1);
  pointers.insert(empty2);

  EXPECT_EQ(pointers.size(), 1);
  EXPECT_TRUE(pointers.find(empty1) != pointers.end());
}
