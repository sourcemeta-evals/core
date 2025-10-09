#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, empty_pointer) {
  const sourcemeta::core::Pointer pointer1;
  const sourcemeta::core::Pointer pointer2;
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
  EXPECT_EQ(hasher(pointer1), 0);
}

TEST(JSONPointer_hash, single_property_equal) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"foo"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, single_property_different) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, single_index_equal) {
  const sourcemeta::core::Pointer pointer1{0};
  const sourcemeta::core::Pointer pointer2{0};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, single_index_different) {
  const sourcemeta::core::Pointer pointer1{0};
  const sourcemeta::core::Pointer pointer2{1};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, multiple_tokens_equal) {
  const sourcemeta::core::Pointer pointer1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", 1, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, multiple_tokens_different) {
  const sourcemeta::core::Pointer pointer1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", 2, "bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, unordered_set_pointer) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "baz"};
  const sourcemeta::core::Pointer pointer3{"foo", "bar"};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(pointer1) != set.end());
  EXPECT_TRUE(set.find(pointer2) != set.end());
  EXPECT_TRUE(set.find(pointer3) != set.end());
}

TEST(JSONPointer_hash, unordered_map_pointer) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "baz"};

  map[pointer1] = 1;
  map[pointer2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(pointer1), 1);
  EXPECT_EQ(map.at(pointer2), 2);
}

TEST(JSONWeakPointer_hash, empty_pointer) {
  const sourcemeta::core::WeakPointer pointer1;
  const sourcemeta::core::WeakPointer pointer2;
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
  EXPECT_EQ(hasher(pointer1), 0);
}

TEST(JSONWeakPointer_hash, single_property_equal) {
  const std::string foo1{"foo"};
  const std::string foo2{"foo"};
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo1)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo2)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONWeakPointer_hash, single_property_different) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONWeakPointer_hash, multiple_tokens_equal) {
  const std::string foo1{"foo"};
  const std::string bar1{"bar"};
  const std::string foo2{"foo"};
  const std::string bar2{"bar"};
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo1), 1,
                                               std::cref(bar1)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo2), 1,
                                               std::cref(bar2)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONWeakPointer_hash, unordered_set_weakpointer) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string baz{"baz"};
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo), std::cref(bar)};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
}
