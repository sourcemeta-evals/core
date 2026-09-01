#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_empty) {
  const sourcemeta::core::Pointer pointer;
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_single_property) {
  const sourcemeta::core::Pointer pointer{"foo"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_single_index) {
  const sourcemeta::core::Pointer pointer{5};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_multiple_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_equal_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer pointer_2{"foo", "bar", "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_different_pointers_different_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  set.insert(sourcemeta::core::Pointer{"foo", "baz"});
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", "bar"}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", "baz"}));
  EXPECT_FALSE(set.contains(sourcemeta::core::Pointer{"foo", "qux"}));
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map[sourcemeta::core::Pointer{"foo", "bar"}] = 1;
  map[sourcemeta::core::Pointer{"foo", "baz"}] = 2;
  map[sourcemeta::core::Pointer{"foo", "bar"}] = 3;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ((map[sourcemeta::core::Pointer{"foo", "bar"}]), 3);
  EXPECT_EQ((map[sourcemeta::core::Pointer{"foo", "baz"}]), 2);
}

TEST(JSONPointer_hash, pointer_empty_in_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{});
  set.insert(sourcemeta::core::Pointer{"foo"});

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo"}));
}

TEST(JSONPointer_hash, pointer_index_tokens_in_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{0});
  set.insert(sourcemeta::core::Pointer{1});
  set.insert(sourcemeta::core::Pointer{0, 1});

  EXPECT_EQ(set.size(), 3);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{0}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{1}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{0, 1}));
}

TEST(JSONPointer_hash, weakpointer_empty) {
  const sourcemeta::core::WeakPointer pointer;
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weakpointer_single_property) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_single_index) {
  const sourcemeta::core::WeakPointer pointer{5};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_multiple_tokens) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), 1,
                                              std::cref(bar)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_equal_pointers_same_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), std::cref(bar),
                                                std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(foo), std::cref(bar),
                                                std::cref(baz)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, weakpointer_unordered_set) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  std::unordered_set<sourcemeta::core::WeakPointer> set;
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});

  EXPECT_EQ(set.size(), 2);
}

TEST(JSONPointer_hash, weakpointer_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  map[sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)}] = 1;
  map[sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)}] = 2;
  map[sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)}] = 3;

  EXPECT_EQ(map.size(), 2);
}

TEST(JSONPointer_hash, pointer_and_weakpointer_same_hash) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const auto weak = sourcemeta::core::to_weak_pointer(pointer);

  std::hash<sourcemeta::core::Pointer> pointer_hasher;
  std::hash<sourcemeta::core::WeakPointer> weak_hasher;

  EXPECT_EQ(pointer_hasher(pointer), weak_hasher(weak));
}

TEST(JSONPointer_hash, pointer_many_tokens) {
  const sourcemeta::core::Pointer pointer{"a", "b", "c", "d", "e",
                                          "f", "g", "h", "i", "j"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_mixed_tokens_in_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{"foo", 0, "bar"});
  set.insert(sourcemeta::core::Pointer{"foo", 1, "bar"});
  set.insert(sourcemeta::core::Pointer{"foo", 0, "baz"});

  EXPECT_EQ(set.size(), 3);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", 0, "bar"}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", 1, "bar"}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", 0, "baz"}));
}
