#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

static const std::string foo = "foo";
static const std::string bar = "bar";
static const std::string baz = "baz";
static const std::string qux = "qux";

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
  const sourcemeta::core::Pointer pointer{1};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 1);
}

TEST(JSONPointer_hash, pointer_equal_same_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_different_likely_different_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"baz", 2, "qux"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  set.insert(sourcemeta::core::Pointer{"foo", "baz"});
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  EXPECT_EQ(set.size(), 2);
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map[sourcemeta::core::Pointer{"foo", "bar"}] = 1;
  map[sourcemeta::core::Pointer{"foo", "baz"}] = 2;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo", "bar"}), 1);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo", "baz"}), 2);
}

TEST(JSONPointer_hash, pointer_two_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_three_tokens_samples_middle) {
  const sourcemeta::core::Pointer pointer_1{"foo", "aaa", "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", "zzz", "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, weak_pointer_empty) {
  const sourcemeta::core::WeakPointer pointer;
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weak_pointer_equal_same_hash) {
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), 1,
                                                std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(foo), 1,
                                                std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, weak_pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  EXPECT_EQ(set.size(), 2);
}

TEST(JSONPointer_hash, weak_pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  map[sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)}] = 1;
  map[sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)}] = 2;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(
      map.at(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)}), 1);
  EXPECT_EQ(
      map.at(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)}), 2);
}
