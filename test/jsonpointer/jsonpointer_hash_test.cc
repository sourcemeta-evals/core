#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <functional>    // std::hash
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
  const auto result{hasher(pointer)};
  // Just check it produces a non-trivial value
  EXPECT_NE(result, 0);
}

TEST(JSONPointer_hash, pointer_single_index) {
  const sourcemeta::core::Pointer pointer{5};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto result{hasher(pointer)};
  EXPECT_NE(result, 0);
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
  // Not guaranteed, but very likely for different pointers
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  set.insert(sourcemeta::core::Pointer{"foo", "baz"});
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(sourcemeta::core::Pointer{"foo", "bar"}) != set.end());
  EXPECT_TRUE(set.find(sourcemeta::core::Pointer{"foo", "baz"}) != set.end());
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map[sourcemeta::core::Pointer{"foo"}] = 1;
  map[sourcemeta::core::Pointer{"bar"}] = 2;
  map[sourcemeta::core::Pointer{"foo"}] = 3;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo"}), 3);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"bar"}), 2);
}

TEST(JSONPointer_hash, pointer_mixed_tokens) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{"foo", 0, "bar"});
  set.insert(sourcemeta::core::Pointer{"foo", 1, "bar"});
  set.insert(sourcemeta::core::Pointer{0});
  set.insert(sourcemeta::core::Pointer{"foo", 0, "bar"});
  EXPECT_EQ(set.size(), 3);
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
  const auto result{hasher(pointer)};
  EXPECT_NE(result, 0);
}

TEST(JSONPointer_hash, weak_pointer_equal_pointers_same_hash) {
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
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)});
  EXPECT_EQ(set.size(), 2);
}

TEST(JSONPointer_hash, pointer_and_weak_pointer_consistent) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  const sourcemeta::core::WeakPointer weak{
      sourcemeta::core::to_weak_pointer(pointer)};
  const std::hash<sourcemeta::core::Pointer> hasher_pointer;
  const std::hash<sourcemeta::core::WeakPointer> hasher_weak;
  EXPECT_EQ(hasher_pointer(pointer), hasher_weak(weak));
}
