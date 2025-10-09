#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <functional>
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
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_single_index) {
  const sourcemeta::core::Pointer pointer{5};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_EQ(hash_value, 5);
}

TEST(JSONPointer_hash, pointer_multiple_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_equal_pointers_equal_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_different_pointers_different_hash) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", 2, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  const sourcemeta::core::Pointer pointer_1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer_2{"baz"};
  const sourcemeta::core::Pointer pointer_3{"foo", "bar"};

  set.insert(pointer_1);
  set.insert(pointer_2);
  set.insert(pointer_3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(pointer_1) != set.end());
  EXPECT_TRUE(set.find(pointer_2) != set.end());
  EXPECT_TRUE(set.find(pointer_3) != set.end());
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  const sourcemeta::core::Pointer pointer_1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer_2{"baz"};

  map[pointer_1] = 42;
  map[pointer_2] = 100;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(pointer_1), 42);
  EXPECT_EQ(map.at(pointer_2), 100);
}

TEST(JSONPointer_hash, pointer_long_path_uses_sampling) {
  const sourcemeta::core::Pointer pointer{"a", "b", "c", "d", "e",
                                          "f", "g", "h", "i", "j"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);

  const sourcemeta::core::Pointer pointer_changed_1{"a", "X", "c", "d", "e",
                                                    "f", "g", "h", "i", "j"};
  EXPECT_EQ(hasher(pointer), hasher(pointer_changed_1));

  const sourcemeta::core::Pointer pointer_changed_first{
      "X", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
  EXPECT_NE(hasher(pointer), hasher(pointer_changed_first));

  const sourcemeta::core::Pointer pointer_changed_last{"a", "b", "c", "d", "e",
                                                       "f", "g", "h", "i", "X"};
  EXPECT_NE(hasher(pointer), hasher(pointer_changed_last));
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
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, weakpointer_single_index) {
  const sourcemeta::core::WeakPointer pointer{5};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_EQ(hash_value, 5);
}

TEST(JSONPointer_hash, weakpointer_multiple_tokens) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), 1,
                                              std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, weakpointer_equal_pointers_equal_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), 1,
                                                std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(foo), 1,
                                                std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer_1), hasher(pointer_2));
}

TEST(JSONPointer_hash, weakpointer_unordered_set) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer_3{std::cref(foo), std::cref(bar)};

  set.insert(pointer_1);
  set.insert(pointer_2);
  set.insert(pointer_3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(pointer_1) != set.end());
  EXPECT_TRUE(set.find(pointer_2) != set.end());
  EXPECT_TRUE(set.find(pointer_3) != set.end());
}

TEST(JSONPointer_hash, weakpointer_unordered_map) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  const sourcemeta::core::WeakPointer pointer_1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer_2{std::cref(baz)};

  map[pointer_1] = 42;
  map[pointer_2] = 100;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(pointer_1), 42);
  EXPECT_EQ(map.at(pointer_2), 100);
}

TEST(JSONPointer_hash, pointer_and_weakpointer_same_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";

  const sourcemeta::core::Pointer pointer{foo, 1, bar};
  const sourcemeta::core::WeakPointer weak_pointer{std::cref(foo), 1,
                                                   std::cref(bar)};

  const std::hash<sourcemeta::core::Pointer> pointer_hasher;
  const std::hash<sourcemeta::core::WeakPointer> weak_hasher;

  EXPECT_EQ(pointer_hasher(pointer), weak_hasher(weak_pointer));
}
