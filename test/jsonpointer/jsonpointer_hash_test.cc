#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, empty_pointer) {
  const sourcemeta::core::Pointer pointer;
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, single_property_token) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, single_index_token) {
  const sourcemeta::core::Pointer pointer{0};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, single_index_token_nonzero) {
  const sourcemeta::core::Pointer pointer{5};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 5);
}

TEST(JSONPointer_hash, two_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, three_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, mixed_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, equal_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer pointer_2{"foo", "bar", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, different_pointers_likely_different_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer_2{"baz", "qux"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, unordered_set_usage) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;
  pointers.insert(sourcemeta::core::Pointer{"foo"});
  pointers.insert(sourcemeta::core::Pointer{"bar"});
  pointers.insert(sourcemeta::core::Pointer{"foo"});

  EXPECT_EQ(pointers.size(), 2);
  EXPECT_TRUE(pointers.contains(sourcemeta::core::Pointer{"foo"}));
  EXPECT_TRUE(pointers.contains(sourcemeta::core::Pointer{"bar"}));
  EXPECT_FALSE(pointers.contains(sourcemeta::core::Pointer{"baz"}));
}

TEST(JSONPointer_hash, unordered_map_usage) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map[sourcemeta::core::Pointer{"foo"}] = 1;
  map[sourcemeta::core::Pointer{"bar", "baz"}] = 2;
  map[sourcemeta::core::Pointer{0, 1, 2}] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo"}), 1);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"bar", "baz"}), 2);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{0, 1, 2}), 3);
}

static const std::string foo = "foo";
static const std::string bar = "bar";
static const std::string baz = "baz";
static const std::string qux = "qux";

TEST(JSONWeakPointer_hash, empty_pointer) {
  const sourcemeta::core::WeakPointer pointer;
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONWeakPointer_hash, single_property_token) {
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONWeakPointer_hash, single_index_token) {
  const sourcemeta::core::WeakPointer pointer{0};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONWeakPointer_hash, single_index_token_nonzero) {
  const sourcemeta::core::WeakPointer pointer{5};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 5);
}

TEST(JSONWeakPointer_hash, two_tokens) {
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONWeakPointer_hash, three_tokens) {
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONWeakPointer_hash, mixed_tokens) {
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), 1,
                                              std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONWeakPointer_hash, equal_pointers_same_hash) {
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), std::cref(bar),
                                                std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(foo), std::cref(bar),
                                                std::cref(baz)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONWeakPointer_hash, different_pointers_likely_different_hash) {
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(baz), std::cref(qux)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONWeakPointer_hash, unordered_set_usage) {
  std::unordered_set<sourcemeta::core::WeakPointer> pointers;
  pointers.insert(sourcemeta::core::WeakPointer{std::cref(foo)});
  pointers.insert(sourcemeta::core::WeakPointer{std::cref(bar)});
  pointers.insert(sourcemeta::core::WeakPointer{std::cref(foo)});

  EXPECT_EQ(pointers.size(), 2);
  EXPECT_TRUE(pointers.contains(sourcemeta::core::WeakPointer{std::cref(foo)}));
  EXPECT_TRUE(pointers.contains(sourcemeta::core::WeakPointer{std::cref(bar)}));
  EXPECT_FALSE(
      pointers.contains(sourcemeta::core::WeakPointer{std::cref(baz)}));
}

TEST(JSONWeakPointer_hash, unordered_map_usage) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  map[sourcemeta::core::WeakPointer{std::cref(foo)}] = 1;
  map[sourcemeta::core::WeakPointer{std::cref(bar), std::cref(baz)}] = 2;
  map[sourcemeta::core::WeakPointer{0, 1, 2}] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(foo)}), 1);
  EXPECT_EQ(
      map.at(sourcemeta::core::WeakPointer{std::cref(bar), std::cref(baz)}), 2);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{0, 1, 2}), 3);
}
