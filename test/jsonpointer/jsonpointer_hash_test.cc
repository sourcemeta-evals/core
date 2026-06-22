#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_empty) {
  const sourcemeta::core::Pointer pointer;
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_single_property) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_single_index) {
  const sourcemeta::core::Pointer pointer{5};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 5);
}

TEST(JSONPointer_hash, pointer_two_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_three_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_mixed_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_equal_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", "baz"};
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
  set.insert(sourcemeta::core::Pointer{"foo"});
  set.insert(sourcemeta::core::Pointer{"bar"});
  set.insert(sourcemeta::core::Pointer{"foo"});
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo"}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"bar"}));
  EXPECT_FALSE(set.contains(sourcemeta::core::Pointer{"baz"}));
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer key1{"foo"};
  const sourcemeta::core::Pointer key2{"bar"};
  const sourcemeta::core::Pointer key3{"foo", "baz"};
  map[key1] = 1;
  map[key2] = 2;
  map[key3] = 3;
  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[key1], 1);
  EXPECT_EQ(map[key2], 2);
  EXPECT_EQ(map[key3], 3);
}

TEST(JSONPointer_hash, weakpointer_empty) {
  const sourcemeta::core::WeakPointer pointer;
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weakpointer_single_property) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weakpointer_single_index) {
  const sourcemeta::core::WeakPointer pointer{5};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 5);
}

TEST(JSONPointer_hash, weakpointer_two_tokens) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weakpointer_three_tokens) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weakpointer_equal_pointers_same_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar),
                                               std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar),
                                               std::cref(baz)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_unordered_set) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(bar)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo)});
  EXPECT_EQ(set.size(), 2);
}

TEST(JSONPointer_hash, weakpointer_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  map[sourcemeta::core::WeakPointer{std::cref(foo)}] = 1;
  map[sourcemeta::core::WeakPointer{std::cref(bar)}] = 2;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[sourcemeta::core::WeakPointer{std::cref(foo)}], 1);
  EXPECT_EQ(map[sourcemeta::core::WeakPointer{std::cref(bar)}], 2);
}

TEST(JSONPointer_hash, pointer_and_weakpointer_same_hash) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer weak_pointer{
      std::cref(foo), std::cref(bar), std::cref(baz)};
  const std::hash<sourcemeta::core::Pointer> pointer_hasher;
  const std::hash<sourcemeta::core::WeakPointer> weak_pointer_hasher;
  EXPECT_EQ(pointer_hasher(pointer), weak_pointer_hasher(weak_pointer));
}
