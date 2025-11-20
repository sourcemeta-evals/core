#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_in_unordered_set_empty) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer;
  set.insert(pointer);
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.count(pointer) == 1);
}

TEST(JSONPointer_hash, pointer_in_unordered_set_single_property) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer{"foo"};
  set.insert(pointer);
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.count(pointer) == 1);
}

TEST(JSONPointer_hash, pointer_in_unordered_set_single_index) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer{42};
  set.insert(pointer);
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.count(pointer) == 1);
}

TEST(JSONPointer_hash, pointer_in_unordered_set_multiple_tokens) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  set.insert(pointer);
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.count(pointer) == 1);
}

TEST(JSONPointer_hash, pointer_in_unordered_set_duplicates) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar"};
  set.insert(pointer1);
  set.insert(pointer2);
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.count(pointer1) == 1);
  EXPECT_TRUE(set.count(pointer2) == 1);
}

TEST(JSONPointer_hash, pointer_in_unordered_set_different_pointers) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const sourcemeta::core::Pointer pointer3{"foo", "bar"};
  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);
  EXPECT_EQ(set.size(), 3);
  EXPECT_TRUE(set.count(pointer1) == 1);
  EXPECT_TRUE(set.count(pointer2) == 1);
  EXPECT_TRUE(set.count(pointer3) == 1);
}

TEST(JSONPointer_hash, pointer_in_unordered_map_basic) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  map[pointer] = 42;
  EXPECT_EQ(map.size(), 1);
  EXPECT_EQ(map[pointer], 42);
}

TEST(JSONPointer_hash, pointer_in_unordered_map_multiple_entries) {
  std::unordered_map<sourcemeta::core::Pointer, std::string> map;
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const sourcemeta::core::Pointer pointer3{"foo", "bar"};

  map[pointer1] = "first";
  map[pointer2] = "second";
  map[pointer3] = "third";

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[pointer1], "first");
  EXPECT_EQ(map[pointer2], "second");
  EXPECT_EQ(map[pointer3], "third");
}

TEST(JSONPointer_hash, pointer_equal_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", 1, "bar"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_in_unordered_set_empty) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const sourcemeta::core::WeakPointer pointer;
  set.insert(pointer);
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.count(pointer) == 1);
}

TEST(JSONPointer_hash, weakpointer_in_unordered_set_single_property) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  set.insert(pointer);
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.count(pointer) == 1);
}

TEST(JSONPointer_hash, weakpointer_in_unordered_set_single_index) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const sourcemeta::core::WeakPointer pointer{42};
  set.insert(pointer);
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.count(pointer) == 1);
}

TEST(JSONPointer_hash, weakpointer_in_unordered_set_multiple_tokens) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), 1,
                                              std::cref(bar)};
  set.insert(pointer);
  EXPECT_EQ(set.size(), 1);
  EXPECT_TRUE(set.count(pointer) == 1);
}

TEST(JSONPointer_hash, weakpointer_in_unordered_set_different_pointers) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo), std::cref(bar)};
  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);
  EXPECT_EQ(set.size(), 3);
  EXPECT_TRUE(set.count(pointer1) == 1);
  EXPECT_TRUE(set.count(pointer2) == 1);
  EXPECT_TRUE(set.count(pointer3) == 1);
}

TEST(JSONPointer_hash, weakpointer_in_unordered_map_basic) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar)};
  map[pointer] = 42;
  EXPECT_EQ(map.size(), 1);
  EXPECT_EQ(map[pointer], 42);
}

TEST(JSONPointer_hash, weakpointer_in_unordered_map_multiple_entries) {
  std::unordered_map<sourcemeta::core::WeakPointer, std::string> map;
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo), std::cref(bar)};

  map[pointer1] = "first";
  map[pointer2] = "second";
  map[pointer3] = "third";

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[pointer1], "first");
  EXPECT_EQ(map[pointer2], "second");
  EXPECT_EQ(map[pointer3], "third");
}

TEST(JSONPointer_hash, weakpointer_equal_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), 1,
                                               std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), 1,
                                               std::cref(bar)};

  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_different_sizes) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"a"};
  const sourcemeta::core::Pointer pointer2{"a", "b"};
  const sourcemeta::core::Pointer pointer3{"a", "b", "c"};
  const sourcemeta::core::Pointer pointer4{"a", "b", "c", "d"};
  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);
  set.insert(pointer4);
  EXPECT_EQ(set.size(), 4);
}

TEST(JSONPointer_hash, pointer_hash_with_indices) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{0};
  const sourcemeta::core::Pointer pointer2{1};
  const sourcemeta::core::Pointer pointer3{100};
  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);
  EXPECT_EQ(set.size(), 3);
  EXPECT_TRUE(set.count(pointer1) == 1);
  EXPECT_TRUE(set.count(pointer2) == 1);
  EXPECT_TRUE(set.count(pointer3) == 1);
}

TEST(JSONPointer_hash, pointer_hash_mixed_tokens) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo", 0, "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer3{0, "foo", "bar"};
  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);
  EXPECT_EQ(set.size(), 3);
}
