#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_empty_hash_consistency) {
  const sourcemeta::core::Pointer pointer1;
  const sourcemeta::core::Pointer pointer2;
  EXPECT_EQ(std::hash<sourcemeta::core::Pointer>{}(pointer1),
            std::hash<sourcemeta::core::Pointer>{}(pointer2));
}

TEST(JSONPointer_hash, pointer_single_property_hash_consistency) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"foo"};
  EXPECT_EQ(std::hash<sourcemeta::core::Pointer>{}(pointer1),
            std::hash<sourcemeta::core::Pointer>{}(pointer2));
}

TEST(JSONPointer_hash, pointer_single_index_hash_consistency) {
  const sourcemeta::core::Pointer pointer1{5};
  const sourcemeta::core::Pointer pointer2{5};
  EXPECT_EQ(std::hash<sourcemeta::core::Pointer>{}(pointer1),
            std::hash<sourcemeta::core::Pointer>{}(pointer2));
}

TEST(JSONPointer_hash, pointer_multiple_tokens_hash_consistency) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", 3, "baz"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", 3, "baz"};
  EXPECT_EQ(std::hash<sourcemeta::core::Pointer>{}(pointer1),
            std::hash<sourcemeta::core::Pointer>{}(pointer2));
}

TEST(JSONPointer_hash, pointer_different_pointers_different_hash) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  EXPECT_NE(std::hash<sourcemeta::core::Pointer>{}(pointer1),
            std::hash<sourcemeta::core::Pointer>{}(pointer2));
}

TEST(JSONPointer_hash, pointer_unordered_set_basic) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const sourcemeta::core::Pointer pointer3{"foo"};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(pointer1));
  EXPECT_TRUE(set.contains(pointer2));
  EXPECT_TRUE(set.contains(pointer3));
}

TEST(JSONPointer_hash, pointer_unordered_set_complex) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo", "bar", 1};
  const sourcemeta::core::Pointer pointer2{"foo", "baz", 2};
  const sourcemeta::core::Pointer pointer3{"foo", "bar", 1};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(pointer1));
  EXPECT_TRUE(set.contains(pointer2));
}

TEST(JSONPointer_hash, pointer_unordered_map_basic) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};

  map[pointer1] = 1;
  map[pointer2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(pointer1), 1);
  EXPECT_EQ(map.at(pointer2), 2);
}

TEST(JSONPointer_hash, pointer_unordered_map_overwrite) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer{"foo", "bar"};

  map[pointer] = 1;
  map[pointer] = 2;

  EXPECT_EQ(map.size(), 1);
  EXPECT_EQ(map.at(pointer), 2);
}

TEST(JSONPointer_hash, weakpointer_hash_consistency) {
  static const std::string foo = "foo";
  static const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar)};
  EXPECT_EQ(std::hash<sourcemeta::core::WeakPointer>{}(pointer1),
            std::hash<sourcemeta::core::WeakPointer>{}(pointer2));
}

TEST(JSONPointer_hash, weakpointer_unordered_set) {
  static const std::string foo = "foo";
  static const std::string bar = "bar";
  static const std::string baz = "baz";

  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo)};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(pointer1));
  EXPECT_TRUE(set.contains(pointer2));
}

TEST(JSONPointer_hash, weakpointer_unordered_map) {
  static const std::string foo = "foo";
  static const std::string bar = "bar";

  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};

  map[pointer1] = 1;
  map[pointer2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.at(pointer1), 1);
  EXPECT_EQ(map.at(pointer2), 2);
}
