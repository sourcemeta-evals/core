#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <functional>    // std::hash, std::reference_wrapper, std::cref
#include <string>        // std::string
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set

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
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_equal_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_different_pointers_likely_different_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"baz", 2, "qux"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_unordered_set) {
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

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> pointer_map;
  pointer_map[sourcemeta::core::Pointer{"foo", "bar"}] = 1;
  pointer_map[sourcemeta::core::Pointer{"foo", "baz"}] = 2;
  pointer_map[sourcemeta::core::Pointer{"foo", "bar"}] = 3;
  EXPECT_EQ(pointer_map.size(), 2);
  EXPECT_EQ(pointer_map.at(sourcemeta::core::Pointer{"foo", "bar"}), 3);
  EXPECT_EQ(pointer_map.at(sourcemeta::core::Pointer{"foo", "baz"}), 2);
}

TEST(JSONPointer_hash, pointer_empty_in_set) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;
  pointers.insert(sourcemeta::core::Pointer{});
  pointers.insert(sourcemeta::core::Pointer{"foo"});
  pointers.insert(sourcemeta::core::Pointer{});
  EXPECT_EQ(pointers.size(), 2);
}

TEST(JSONPointer_hash, pointer_index_only) {
  const sourcemeta::core::Pointer pointer_1{0};
  const sourcemeta::core::Pointer pointer_2{1};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_mixed_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value{hasher(pointer)};
  EXPECT_EQ(hasher(pointer), hash_value);
}

TEST(JSONPointer_hash, weak_pointer_empty) {
  const sourcemeta::core::WeakPointer pointer;
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weak_pointer_single_property) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weak_pointer_equal_same_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), 1,
                                                std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(foo), 1,
                                                std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, weak_pointer_unordered_set) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  std::unordered_set<sourcemeta::core::WeakPointer> pointers;
  pointers.insert(
      sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  pointers.insert(
      sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)});
  pointers.insert(
      sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  EXPECT_EQ(pointers.size(), 2);
}

TEST(JSONPointer_hash, weak_pointer_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  std::unordered_map<sourcemeta::core::WeakPointer, int> pointer_map;
  pointer_map[sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)}] =
      1;
  pointer_map[sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)}] =
      2;
  pointer_map[sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)}] =
      3;
  EXPECT_EQ(pointer_map.size(), 2);
  EXPECT_EQ(pointer_map.at(
                sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)}),
            3);
  EXPECT_EQ(pointer_map.at(
                sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)}),
            2);
}
