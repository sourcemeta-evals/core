#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

static const std::string foo = "foo";
static const std::string bar = "bar";
static const std::string baz = "baz";
static const std::string qux = "qux";

TEST(JSONPointer_hash, unordered_map_insert_and_find) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"baz"};

  map[pointer1] = 42;
  map[pointer2] = 100;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 42);
  EXPECT_EQ(map[pointer2], 100);

  auto it = map.find(pointer1);
  EXPECT_NE(it, map.end());
  EXPECT_EQ(it->second, 42);
}

TEST(JSONPointer_hash, unordered_map_empty_pointer) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer empty_pointer;

  map[empty_pointer] = 0;

  EXPECT_EQ(map.size(), 1);
  EXPECT_EQ(map[empty_pointer], 0);
}

TEST(JSONPointer_hash, unordered_map_single_token) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer{"foo"};

  map[pointer] = 1;

  EXPECT_EQ(map.size(), 1);
  EXPECT_EQ(map[pointer], 1);
}

TEST(JSONPointer_hash, unordered_map_with_index_tokens) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer1{"foo", 0, "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", 1, "bar"};

  map[pointer1] = 10;
  map[pointer2] = 20;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 10);
  EXPECT_EQ(map[pointer2], 20);
}

TEST(JSONPointer_hash, unordered_map_index_only) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer1{0};
  const sourcemeta::core::Pointer pointer2{1};
  const sourcemeta::core::Pointer pointer3{100};

  map[pointer1] = 1;
  map[pointer2] = 2;
  map[pointer3] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[pointer1], 1);
  EXPECT_EQ(map[pointer2], 2);
  EXPECT_EQ(map[pointer3], 3);
}

TEST(JSONPointer_hash, unordered_set_insert_and_contains) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"baz"};

  set.insert(pointer1);
  set.insert(pointer2);

  EXPECT_EQ(set.size(), 2);
  EXPECT_NE(set.find(pointer1), set.end());
  EXPECT_NE(set.find(pointer2), set.end());
}

TEST(JSONPointer_hash, unordered_set_duplicate_insert) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer{"foo", "bar"};

  set.insert(pointer);
  set.insert(pointer);

  EXPECT_EQ(set.size(), 1);
}

TEST(JSONPointer_hash, unordered_set_different_pointers_no_alias) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "baz"};
  const sourcemeta::core::Pointer pointer3{"bar", "foo"};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 3);
}

TEST(JSONWeakPointer_hash, unordered_map_insert_and_find) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz)};

  map[pointer1] = 42;
  map[pointer2] = 100;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 42);
  EXPECT_EQ(map[pointer2], 100);

  auto it = map.find(pointer1);
  EXPECT_NE(it, map.end());
  EXPECT_EQ(it->second, 42);
}

TEST(JSONWeakPointer_hash, unordered_map_empty_pointer) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const sourcemeta::core::WeakPointer empty_pointer;

  map[empty_pointer] = 0;

  EXPECT_EQ(map.size(), 1);
  EXPECT_EQ(map[empty_pointer], 0);
}

TEST(JSONWeakPointer_hash, unordered_map_single_token) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};

  map[pointer] = 1;

  EXPECT_EQ(map.size(), 1);
  EXPECT_EQ(map[pointer], 1);
}

TEST(JSONWeakPointer_hash, unordered_map_with_index_tokens) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), 0,
                                               std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), 1,
                                               std::cref(bar)};

  map[pointer1] = 10;
  map[pointer2] = 20;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 10);
  EXPECT_EQ(map[pointer2], 20);
}

TEST(JSONWeakPointer_hash, unordered_set_insert_and_contains) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(baz)};

  set.insert(pointer1);
  set.insert(pointer2);

  EXPECT_EQ(set.size(), 2);
  EXPECT_NE(set.find(pointer1), set.end());
  EXPECT_NE(set.find(pointer2), set.end());
}

TEST(JSONWeakPointer_hash, unordered_set_duplicate_insert) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar)};

  set.insert(pointer);
  set.insert(pointer);

  EXPECT_EQ(set.size(), 1);
}

TEST(JSONWeakPointer_hash, unordered_set_equivalent_instances) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar)};

  set.insert(pointer1);
  set.insert(pointer2);

  EXPECT_EQ(set.size(), 1);
}

TEST(JSONWeakPointer_hash, unordered_set_different_pointers_no_alias) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(bar), std::cref(foo)};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 3);
}

TEST(JSONPointer_hash, hyphen_token) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer1{"foo", "-"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar"};

  map[pointer1] = 1;
  map[pointer2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 1);
  EXPECT_EQ(map[pointer2], 2);
}

TEST(JSONPointer_hash, multiple_tokens_sampling) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer1{"a", "b", "c", "d", "e"};
  const sourcemeta::core::Pointer pointer2{"a", "b", "x", "d", "e"};
  const sourcemeta::core::Pointer pointer3{"x", "b", "c", "d", "e"};
  const sourcemeta::core::Pointer pointer4{"a", "b", "c", "d", "x"};

  map[pointer1] = 1;
  map[pointer2] = 2;
  map[pointer3] = 3;
  map[pointer4] = 4;

  EXPECT_EQ(map.size(), 4);
  EXPECT_EQ(map[pointer1], 1);
  EXPECT_EQ(map[pointer2], 2);
  EXPECT_EQ(map[pointer3], 3);
  EXPECT_EQ(map[pointer4], 4);
}
