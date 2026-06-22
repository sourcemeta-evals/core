#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, empty_pointer) {
  const sourcemeta::core::Pointer pointer;
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, equal_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, different_pointers_likely_different_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"baz", 2, "qux"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, single_property_token) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, single_index_token) {
  const sourcemeta::core::Pointer pointer{5};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, two_tokens) {
  const sourcemeta::core::Pointer pointer_1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  set.insert(sourcemeta::core::Pointer{"foo", "baz"});
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", "bar"}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", "baz"}));
  EXPECT_FALSE(set.contains(sourcemeta::core::Pointer{"other"}));
}

TEST(JSONPointer_hash, unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map[sourcemeta::core::Pointer{"foo"}] = 1;
  map[sourcemeta::core::Pointer{"bar"}] = 2;
  map[sourcemeta::core::Pointer{"foo"}] = 3;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo"}), 3);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"bar"}), 2);
}

static const std::string foo = "foo";
static const std::string bar = "bar";
static const std::string baz = "baz";
static const std::string qux = "qux";
static const std::string other = "other";

TEST(JSONWeakPointer_hash, empty_pointer) {
  const sourcemeta::core::WeakPointer pointer;
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONWeakPointer_hash, equal_pointers_same_hash) {
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), 1,
                                                std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(foo), 1,
                                                std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONWeakPointer_hash, different_pointers_likely_different_hash) {
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), 1,
                                                std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(baz), 2,
                                                std::cref(qux)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONWeakPointer_hash, single_property_token) {
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONWeakPointer_hash, single_index_token) {
  const sourcemeta::core::WeakPointer pointer{5};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONWeakPointer_hash, two_tokens) {
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(foo), std::cref(baz)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONWeakPointer_hash, unordered_set) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(
      sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)}));
  EXPECT_TRUE(set.contains(
      sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)}));
  EXPECT_FALSE(set.contains(sourcemeta::core::WeakPointer{std::cref(other)}));
}

TEST(JSONWeakPointer_hash, unordered_map) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  map[sourcemeta::core::WeakPointer{std::cref(foo)}] = 1;
  map[sourcemeta::core::WeakPointer{std::cref(bar)}] = 2;
  map[sourcemeta::core::WeakPointer{std::cref(foo)}] = 3;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(foo)}), 3);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(bar)}), 2);
}
