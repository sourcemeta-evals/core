#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_empty) {
  const sourcemeta::core::Pointer pointer;
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), hasher(pointer));
}

TEST(JSONPointer_hash, pointer_single_property) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), hasher(pointer));
}

TEST(JSONPointer_hash, pointer_single_index) {
  const sourcemeta::core::Pointer pointer{0};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), hasher(pointer));
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
  // Not guaranteed, but highly likely for different pointers
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
  EXPECT_FALSE(set.contains(sourcemeta::core::Pointer{"other"}));
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map[sourcemeta::core::Pointer{"foo", "bar"}] = 1;
  map[sourcemeta::core::Pointer{"foo", "baz"}] = 2;
  map[sourcemeta::core::Pointer{"foo", "bar"}] = 3;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo", "bar"}), 3);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo", "baz"}), 2);
}

TEST(JSONPointer_hash, pointer_unordered_set_with_indices) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{0, "foo"});
  set.insert(sourcemeta::core::Pointer{1, "foo"});
  set.insert(sourcemeta::core::Pointer{0, "foo"});
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{0, "foo"}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{1, "foo"}));
}

TEST(JSONPointer_hash, pointer_unordered_set_empty_pointer) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{});
  set.insert(sourcemeta::core::Pointer{"foo"});
  set.insert(sourcemeta::core::Pointer{});
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo"}));
}

TEST(JSONPointer_hash, weak_pointer_empty) {
  const sourcemeta::core::WeakPointer pointer;
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), hasher(pointer));
}

TEST(JSONPointer_hash, weak_pointer_single_property) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), hasher(pointer));
}

TEST(JSONPointer_hash, weak_pointer_single_index) {
  const sourcemeta::core::WeakPointer pointer{0};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), hasher(pointer));
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

TEST(JSONPointer_hash, weak_pointer_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  map[sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)}] = 1;
  map[sourcemeta::core::WeakPointer{std::cref(foo), std::cref(baz)}] = 2;
  map[sourcemeta::core::WeakPointer{std::cref(foo), std::cref(bar)}] = 3;
  EXPECT_EQ(map.size(), 2);
}

TEST(JSONPointer_hash, pointer_and_weak_pointer_consistent_hash) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  const sourcemeta::core::WeakPointer weak{
      sourcemeta::core::to_weak_pointer(pointer)};
  const std::hash<sourcemeta::core::Pointer> pointer_hasher;
  const std::hash<sourcemeta::core::WeakPointer> weak_hasher;
  // Both should produce the same hash for equivalent pointers
  EXPECT_EQ(pointer_hasher(pointer), weak_hasher(weak));
}
