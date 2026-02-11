#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <functional>
#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, empty_pointer) {
  const sourcemeta::core::Pointer pointer;
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, single_property) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, single_index) {
  const sourcemeta::core::Pointer pointer{1};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, equal_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, different_pointers_different_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer_2{"baz", "qux"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, unordered_set_pointer) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;
  pointers.insert(sourcemeta::core::Pointer{"foo", "bar"});
  pointers.insert(sourcemeta::core::Pointer{"foo", "baz"});
  pointers.insert(sourcemeta::core::Pointer{"foo", "bar"});
  EXPECT_EQ(pointers.size(), 2);
  EXPECT_TRUE(pointers.find(sourcemeta::core::Pointer{"foo", "bar"}) !=
              pointers.end());
  EXPECT_TRUE(pointers.find(sourcemeta::core::Pointer{"foo", "baz"}) !=
              pointers.end());
}

TEST(JSONPointer_hash, unordered_map_pointer) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map[sourcemeta::core::Pointer{"foo", "bar"}] = 1;
  map[sourcemeta::core::Pointer{"foo", "baz"}] = 2;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo", "bar"}), 1);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo", "baz"}), 2);
}

TEST(JSONPointer_hash, index_zero) {
  const sourcemeta::core::Pointer pointer{0};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, three_tokens_uses_middle) {
  const sourcemeta::core::Pointer pointer_1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer pointer_2{"foo", "qux", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
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

TEST(JSONWeakPointer_hash, single_property) {
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONWeakPointer_hash, single_index) {
  const sourcemeta::core::WeakPointer pointer{1};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONWeakPointer_hash, equal_pointers_same_hash) {
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), 1,
                                                std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(foo), 1,
                                                std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONWeakPointer_hash, different_pointers_different_hash) {
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(baz), std::cref(qux)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONWeakPointer_hash, unordered_set_weak_pointer) {
  std::unordered_set<sourcemeta::core::WeakPointer> pointers;
  pointers.insert(
      sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  pointers.insert(
      sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)});
  pointers.insert(
      sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  EXPECT_EQ(pointers.size(), 2);
}

TEST(JSONWeakPointer_hash, unordered_map_weak_pointer) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  map[sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)}] = 1;
  map[sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)}] = 2;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(
      map.at(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)}), 1);
  EXPECT_EQ(
      map.at(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)}), 2);
}
