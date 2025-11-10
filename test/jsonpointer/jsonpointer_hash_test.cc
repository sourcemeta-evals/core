#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

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
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_multiple_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", 1};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_same_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", 1};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", 1};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_different_pointers_different_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", 1};
  const sourcemeta::core::Pointer pointer2{"foo", "baz", 1};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_different_order_different_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"bar", "foo"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_different_sizes_different_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> pointer_set;

  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"baz", "qux"};
  const sourcemeta::core::Pointer pointer3{"foo", "bar"};

  pointer_set.insert(pointer1);
  pointer_set.insert(pointer2);
  pointer_set.insert(pointer3);

  EXPECT_EQ(pointer_set.size(), 2);
  EXPECT_TRUE(pointer_set.find(pointer1) != pointer_set.end());
  EXPECT_TRUE(pointer_set.find(pointer2) != pointer_set.end());
  EXPECT_TRUE(pointer_set.find(pointer3) != pointer_set.end());
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> pointer_map;

  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"baz", "qux"};

  pointer_map[pointer1] = 42;
  pointer_map[pointer2] = 99;

  EXPECT_EQ(pointer_map.size(), 2);
  EXPECT_EQ(pointer_map[pointer1], 42);
  EXPECT_EQ(pointer_map[pointer2], 99);
}

TEST(JSONPointer_hash, pointer_unordered_map_lookup) {
  std::unordered_map<sourcemeta::core::Pointer, std::string> pointer_map;

  const sourcemeta::core::Pointer pointer1{"foo", "bar", 1};
  const sourcemeta::core::Pointer pointer2{"baz"};

  pointer_map[pointer1] = "first";
  pointer_map[pointer2] = "second";

  const sourcemeta::core::Pointer lookup1{"foo", "bar", 1};
  const sourcemeta::core::Pointer lookup2{"baz"};

  EXPECT_EQ(pointer_map[lookup1], "first");
  EXPECT_EQ(pointer_map[lookup2], "second");
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
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, weakpointer_multiple_tokens) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              1};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, weakpointer_same_pointers_same_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar),
                                               1};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar),
                                               1};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_different_pointers_different_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar),
                                               1};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(baz),
                                               1};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_unordered_set) {
  std::unordered_set<sourcemeta::core::WeakPointer> pointer_set;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const std::string qux = "qux";

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz), std::cref(qux)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo), std::cref(bar)};

  pointer_set.insert(pointer1);
  pointer_set.insert(pointer2);
  pointer_set.insert(pointer3);

  EXPECT_EQ(pointer_set.size(), 2);
  EXPECT_TRUE(pointer_set.find(pointer1) != pointer_set.end());
  EXPECT_TRUE(pointer_set.find(pointer2) != pointer_set.end());
  EXPECT_TRUE(pointer_set.find(pointer3) != pointer_set.end());
}

TEST(JSONPointer_hash, weakpointer_unordered_map) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> pointer_map;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const std::string qux = "qux";

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz), std::cref(qux)};

  pointer_map[pointer1] = 42;
  pointer_map[pointer2] = 99;

  EXPECT_EQ(pointer_map.size(), 2);
  EXPECT_EQ(pointer_map[pointer1], 42);
  EXPECT_EQ(pointer_map[pointer2], 99);
}

TEST(JSONPointer_hash, pointer_and_weakpointer_same_content_same_hash) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", 1};

  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer weak_pointer{std::cref(foo),
                                                   std::cref(bar), 1};

  const std::hash<sourcemeta::core::Pointer> pointer_hasher;
  const std::hash<sourcemeta::core::WeakPointer> weak_hasher;

  EXPECT_EQ(pointer_hasher(pointer), weak_hasher(weak_pointer));
}

TEST(JSONPointer_hash, pointer_mixed_property_and_index) {
  const sourcemeta::core::Pointer pointer1{"foo", 0, "bar", 1};
  const sourcemeta::core::Pointer pointer2{"foo", 0, "baz", 1};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_large_pointer) {
  const sourcemeta::core::Pointer pointer{"a", "b", "c", "d",
                                          "e", "f", "g", "h"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, pointer_hash_consistency) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", 1};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1 = hasher(pointer);
  const auto hash2 = hasher(pointer);
  const auto hash3 = hasher(pointer);
  EXPECT_EQ(hash1, hash2);
  EXPECT_EQ(hash2, hash3);
}
