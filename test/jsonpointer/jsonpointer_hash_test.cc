#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_empty_hash) {
  const sourcemeta::core::Pointer pointer;
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_single_property) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_single_index) {
  const sourcemeta::core::Pointer pointer{42};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_EQ(hash, 42);
}

TEST(JSONPointer_hash, pointer_multiple_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_equal_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_different_pointers_different_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
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
  const sourcemeta::core::Pointer pointer3{"baz"};

  map[pointer1] = 1;
  map[pointer2] = 2;
  map[pointer3] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[pointer1], 1);
  EXPECT_EQ(map[pointer2], 2);
  EXPECT_EQ(map[pointer3], 3);
}

TEST(JSONPointer_hash, pointer_unordered_map_complex) {
  std::unordered_map<sourcemeta::core::Pointer, std::string> map;

  const sourcemeta::core::Pointer pointer1{"foo", 0, "bar"};
  const sourcemeta::core::Pointer pointer2{"baz", 1, "qux"};
  const sourcemeta::core::Pointer pointer3{"foo", 0, "bar"};

  map[pointer1] = "first";
  map[pointer2] = "second";
  map[pointer3] = "updated";

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], "updated");
  EXPECT_EQ(map[pointer2], "second");
}

TEST(JSONPointer_hash, weakpointer_empty_hash) {
  const sourcemeta::core::WeakPointer pointer;
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weakpointer_single_property) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_single_index) {
  const sourcemeta::core::WeakPointer pointer{42};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_EQ(hash, 42);
}

TEST(JSONPointer_hash, weakpointer_multiple_tokens) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), 1,
                                              std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_equal_pointers_same_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_different_pointers_different_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(baz)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_unordered_set) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;

  const std::string foo = "foo";
  const std::string bar = "bar";
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

  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(baz)};

  map[pointer1] = 1;
  map[pointer2] = 2;
  map[pointer3] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[pointer1], 1);
  EXPECT_EQ(map[pointer2], 2);
  EXPECT_EQ(map[pointer3], 3);
}

TEST(JSONPointer_hash, weakpointer_unordered_map_complex) {
  std::unordered_map<sourcemeta::core::WeakPointer, std::string> map;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const std::string qux = "qux";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), 0,
                                               std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz), 1,
                                               std::cref(qux)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo), 0,
                                               std::cref(bar)};

  map[pointer1] = "first";
  map[pointer2] = "second";
  map[pointer3] = "updated";

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], "updated");
  EXPECT_EQ(map[pointer2], "second");
}

TEST(JSONPointer_hash, pointer_hash_consistency) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar", 2, "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1 = hasher(pointer);
  const auto hash2 = hasher(pointer);
  EXPECT_EQ(hash1, hash2);
}

TEST(JSONPointer_hash, weakpointer_hash_consistency) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), 1, std::cref(bar),
                                              2, std::cref(baz)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash1 = hasher(pointer);
  const auto hash2 = hasher(pointer);
  EXPECT_EQ(hash1, hash2);
}
