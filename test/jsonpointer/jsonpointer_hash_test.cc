#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <functional>
#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, empty_pointer) {
  const sourcemeta::core::Pointer pointer;
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(JSONPointer_hash, single_property_token) {
  const sourcemeta::core::Pointer pointer{"foo"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, single_index_token) {
  const sourcemeta::core::Pointer pointer{5};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_EQ(hash_value, 5);
}

TEST(JSONPointer_hash, two_property_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, two_index_tokens) {
  const sourcemeta::core::Pointer pointer{1, 2};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, mixed_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, three_or_more_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz", "qux"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(JSONPointer_hash, consistent_hashing) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, different_pointers_different_hashes) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, unordered_set_basic) {
  std::unordered_set<sourcemeta::core::Pointer> pointer_set;

  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const sourcemeta::core::Pointer pointer3{"foo"};

  pointer_set.insert(pointer1);
  pointer_set.insert(pointer2);
  pointer_set.insert(pointer3);

  EXPECT_EQ(pointer_set.size(), 2);
  EXPECT_TRUE(pointer_set.find(pointer1) != pointer_set.end());
  EXPECT_TRUE(pointer_set.find(pointer2) != pointer_set.end());
  EXPECT_TRUE(pointer_set.find(pointer3) != pointer_set.end());
}

TEST(JSONPointer_hash, unordered_set_complex) {
  std::unordered_set<sourcemeta::core::Pointer> pointer_set;

  const sourcemeta::core::Pointer pointer1{"foo", "bar", 1};
  const sourcemeta::core::Pointer pointer2{"foo", "baz", 2};
  const sourcemeta::core::Pointer pointer3{"foo", "bar", 1};

  pointer_set.insert(pointer1);
  pointer_set.insert(pointer2);
  pointer_set.insert(pointer3);

  EXPECT_EQ(pointer_set.size(), 2);
  EXPECT_TRUE(pointer_set.find(pointer1) != pointer_set.end());
  EXPECT_TRUE(pointer_set.find(pointer2) != pointer_set.end());
}

TEST(JSONPointer_hash, unordered_map_basic) {
  std::unordered_map<sourcemeta::core::Pointer, int> pointer_map;

  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};

  pointer_map[pointer1] = 1;
  pointer_map[pointer2] = 2;

  EXPECT_EQ(pointer_map.size(), 2);
  EXPECT_EQ(pointer_map[pointer1], 1);
  EXPECT_EQ(pointer_map[pointer2], 2);
}

TEST(JSONPointer_hash, unordered_map_complex) {
  std::unordered_map<sourcemeta::core::Pointer, std::string> pointer_map;

  const sourcemeta::core::Pointer pointer1{"foo", "bar", 1};
  const sourcemeta::core::Pointer pointer2{"foo", "baz", 2};
  const sourcemeta::core::Pointer pointer3{"qux", 0, "test"};

  pointer_map[pointer1] = "first";
  pointer_map[pointer2] = "second";
  pointer_map[pointer3] = "third";

  EXPECT_EQ(pointer_map.size(), 3);
  EXPECT_EQ(pointer_map[pointer1], "first");
  EXPECT_EQ(pointer_map[pointer2], "second");
  EXPECT_EQ(pointer_map[pointer3], "third");
}

TEST(JSONPointer_hash, unordered_map_update) {
  std::unordered_map<sourcemeta::core::Pointer, int> pointer_map;

  const sourcemeta::core::Pointer pointer{"foo", "bar"};

  pointer_map[pointer] = 1;
  EXPECT_EQ(pointer_map[pointer], 1);

  pointer_map[pointer] = 2;
  EXPECT_EQ(pointer_map[pointer], 2);
  EXPECT_EQ(pointer_map.size(), 1);
}

TEST(WeakPointer_hash, empty_pointer) {
  const sourcemeta::core::WeakPointer pointer;
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer), 0);
}

TEST(WeakPointer_hash, single_property_token) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(WeakPointer_hash, single_index_token) {
  const sourcemeta::core::WeakPointer pointer{5};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_EQ(hash_value, 5);
}

TEST(WeakPointer_hash, two_property_tokens) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(WeakPointer_hash, mixed_tokens) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), 1,
                                              std::cref(bar)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash_value = hasher(pointer);
  EXPECT_NE(hash_value, 0);
}

TEST(WeakPointer_hash, consistent_hashing) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar),
                                               std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar),
                                               std::cref(baz)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(WeakPointer_hash, unordered_set_basic) {
  std::unordered_set<sourcemeta::core::WeakPointer> pointer_set;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo)};

  pointer_set.insert(pointer1);
  pointer_set.insert(pointer2);
  pointer_set.insert(pointer3);

  EXPECT_EQ(pointer_set.size(), 2);
  EXPECT_TRUE(pointer_set.find(pointer1) != pointer_set.end());
  EXPECT_TRUE(pointer_set.find(pointer2) != pointer_set.end());
}

TEST(WeakPointer_hash, unordered_map_basic) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> pointer_map;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};

  pointer_map[pointer1] = 1;
  pointer_map[pointer2] = 2;

  EXPECT_EQ(pointer_map.size(), 2);
  EXPECT_EQ(pointer_map[pointer1], 1);
  EXPECT_EQ(pointer_map[pointer2], 2);
}

TEST(WeakPointer_hash, unordered_map_complex) {
  std::unordered_map<sourcemeta::core::WeakPointer, std::string> pointer_map;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const std::string qux = "qux";
  const std::string test = "test";

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar),
                                               1};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(baz),
                                               2};
  const sourcemeta::core::WeakPointer pointer3{std::cref(qux), 0,
                                               std::cref(test)};

  pointer_map[pointer1] = "first";
  pointer_map[pointer2] = "second";
  pointer_map[pointer3] = "third";

  EXPECT_EQ(pointer_map.size(), 3);
  EXPECT_EQ(pointer_map[pointer1], "first");
  EXPECT_EQ(pointer_map[pointer2], "second");
  EXPECT_EQ(pointer_map[pointer3], "third");
}
