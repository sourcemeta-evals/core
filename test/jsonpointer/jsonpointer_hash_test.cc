#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_empty_hash) {
  const sourcemeta::core::Pointer pointer;
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_equal_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_single_property_token) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto result{hasher(pointer)};
  (void)result;
}

TEST(JSONPointer_hash, pointer_single_index_token) {
  const sourcemeta::core::Pointer pointer{5};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), static_cast<std::size_t>(5));
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  set.insert(sourcemeta::core::Pointer{"baz"});
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", "bar"}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"baz"}));
  EXPECT_FALSE(set.contains(sourcemeta::core::Pointer{"qux"}));
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map[sourcemeta::core::Pointer{"foo", 0}] = 1;
  map[sourcemeta::core::Pointer{"bar", 1}] = 2;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo", 0}), 1);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"bar", 1}), 2);
}

TEST(JSONPointer_hash, pointer_many_tokens) {
  const sourcemeta::core::Pointer pointer{"a", "b", "c", "d", "e"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto result{hasher(pointer)};
  (void)result;
}

static const std::string foo = "foo";
static const std::string bar = "bar";
static const std::string baz = "baz";
static const std::string qux = "qux";

TEST(JSONPointer_hash, weak_pointer_empty_hash) {
  const sourcemeta::core::WeakPointer pointer;
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weak_pointer_equal_pointers_same_hash) {
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), 1,
                                                std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(foo), 1,
                                                std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, weak_pointer_single_index_token) {
  const sourcemeta::core::WeakPointer pointer{5};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), static_cast<std::size_t>(5));
}

TEST(JSONPointer_hash, weak_pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(baz)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(
      sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)}));
  EXPECT_TRUE(set.contains(sourcemeta::core::WeakPointer{std::cref(baz)}));
  EXPECT_FALSE(set.contains(sourcemeta::core::WeakPointer{std::cref(qux)}));
}

TEST(JSONPointer_hash, weak_pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  map[sourcemeta::core::WeakPointer{std::cref(foo), 0}] = 1;
  map[sourcemeta::core::WeakPointer{std::cref(bar), 1}] = 2;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(foo), 0}), 1);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(bar), 1}), 2);
}
