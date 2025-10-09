#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, unordered_set_pointer) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer ptr1{"foo", "bar"};
  const sourcemeta::core::Pointer ptr2{"baz"};
  const sourcemeta::core::Pointer ptr3{"foo", "bar"};

  set.insert(ptr1);
  set.insert(ptr2);
  set.insert(ptr3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(ptr1) != set.end());
  EXPECT_TRUE(set.find(ptr2) != set.end());
  EXPECT_TRUE(set.find(ptr3) != set.end());
}

TEST(JSONPointer_hash, unordered_set_weakpointer) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const sourcemeta::core::WeakPointer ptr1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer ptr2{std::cref(baz)};
  const sourcemeta::core::WeakPointer ptr3{std::cref(foo), std::cref(bar)};

  set.insert(ptr1);
  set.insert(ptr2);
  set.insert(ptr3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(ptr1) != set.end());
  EXPECT_TRUE(set.find(ptr2) != set.end());
  EXPECT_TRUE(set.find(ptr3) != set.end());
}

TEST(JSONPointer_hash, unordered_map_pointer) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer ptr1{"foo", "bar"};
  const sourcemeta::core::Pointer ptr2{"baz"};

  map[ptr1] = 1;
  map[ptr2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(ptr1), 1);
  EXPECT_EQ(map.at(ptr2), 2);
}

TEST(JSONPointer_hash, unordered_map_weakpointer) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const sourcemeta::core::WeakPointer ptr1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer ptr2{std::cref(baz)};

  map[ptr1] = 1;
  map[ptr2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(ptr1), 1);
  EXPECT_EQ(map.at(ptr2), 2);
}

TEST(JSONPointer_hash, equal_pointers_equal_hash) {
  const sourcemeta::core::Pointer ptr1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer ptr2{"foo", "bar", "baz"};

  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(ptr1), hasher(ptr2));
}

TEST(JSONPointer_hash, empty_pointer) {
  const sourcemeta::core::Pointer ptr;
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(ptr), 0);
}

TEST(JSONPointer_hash, single_token_property) {
  const sourcemeta::core::Pointer ptr{"foo"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_GT(hasher(ptr), 0);
}

TEST(JSONPointer_hash, single_token_index) {
  const sourcemeta::core::Pointer ptr{5};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(ptr), 5);
}

TEST(JSONPointer_hash, mixed_tokens) {
  const sourcemeta::core::Pointer ptr{"foo", 1, "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_GT(hasher(ptr), 0);
}

TEST(JSONPointer_hash, weakpointer_equal_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";

  const sourcemeta::core::WeakPointer ptr1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer ptr2{std::cref(foo), std::cref(bar)};

  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(ptr1), hasher(ptr2));
}
