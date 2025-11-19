#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_empty) {
  const sourcemeta::core::Pointer pointer;
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, pointer_single_property) {
  const sourcemeta::core::Pointer pointer{"foo"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_single_index) {
  const sourcemeta::core::Pointer pointer{5};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_multiple_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", 1};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_same_pointers_same_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", 1};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", 1};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_different_pointers_different_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", 1};
  const sourcemeta::core::Pointer pointer2{"foo", "baz", 1};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_different_order_different_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"bar", "foo"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_different_size_different_hash) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_in_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"baz", "qux"};
  const sourcemeta::core::Pointer pointer3{"foo", "bar"};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(pointer1) != set.end());
  EXPECT_TRUE(set.find(pointer2) != set.end());
  EXPECT_TRUE(set.find(pointer3) != set.end());
}

TEST(JSONPointer_hash, pointer_in_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"baz", "qux"};

  map[pointer1] = 42;
  map[pointer2] = 99;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 42);
  EXPECT_EQ(map[pointer2], 99);
}

TEST(JSONPointer_hash, weakpointer_empty) {
  const sourcemeta::core::WeakPointer pointer;
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, weakpointer_single_property) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_single_index) {
  const sourcemeta::core::WeakPointer pointer{5};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_multiple_tokens) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              1};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_same_pointers_same_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar),
                                               1};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar),
                                               1};
  std::hash<sourcemeta::core::WeakPointer> hasher;
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
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weakpointer_in_unordered_set) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const std::string qux = "qux";

  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz), std::cref(qux)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo), std::cref(bar)};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(pointer1) != set.end());
  EXPECT_TRUE(set.find(pointer2) != set.end());
  EXPECT_TRUE(set.find(pointer3) != set.end());
}

TEST(JSONPointer_hash, weakpointer_in_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const std::string qux = "qux";

  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz), std::cref(qux)};

  map[pointer1] = 42;
  map[pointer2] = 99;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 42);
  EXPECT_EQ(map[pointer2], 99);
}

TEST(JSONPointer_hash, pointer_and_weakpointer_consistency) {
  const std::string foo = "foo";
  const std::string bar = "bar";

  const sourcemeta::core::Pointer pointer{"foo", "bar", 1};
  const sourcemeta::core::WeakPointer weak_pointer{std::cref(foo),
                                                   std::cref(bar), 1};

  std::hash<sourcemeta::core::Pointer> pointer_hasher;
  std::hash<sourcemeta::core::WeakPointer> weak_pointer_hasher;

  EXPECT_EQ(pointer_hasher(pointer), weak_pointer_hasher(weak_pointer));
}
