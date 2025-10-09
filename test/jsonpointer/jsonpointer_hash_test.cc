#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_in_unordered_set) {
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

TEST(JSONPointer_hash, pointer_in_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "baz"};

  map[pointer1] = 42;
  map[pointer2] = 100;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 42);
  EXPECT_EQ(map[pointer2], 100);
}

TEST(JSONPointer_hash, weakpointer_in_unordered_set) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(baz)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo), std::cref(bar)};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(pointer1) != set.end());
  EXPECT_TRUE(set.find(pointer2) != set.end());
}

TEST(JSONPointer_hash, weakpointer_in_unordered_map) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;

  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(baz)};

  map[pointer1] = 42;
  map[pointer2] = 100;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 42);
  EXPECT_EQ(map[pointer2], 100);
}

TEST(JSONPointer_hash, pointer_with_indices) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  const sourcemeta::core::Pointer pointer1{"foo", 0, "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer3{"foo", 0, "bar"};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
}

TEST(JSONPointer_hash, empty_pointer) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  const sourcemeta::core::Pointer empty1;
  const sourcemeta::core::Pointer empty2;

  set.insert(empty1);
  set.insert(empty2);

  EXPECT_EQ(set.size(), 1);
}

TEST(JSONPointer_hash, single_token_pointer) {
  std::unordered_map<sourcemeta::core::Pointer, std::string> map;

  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};

  map[pointer1] = "value1";
  map[pointer2] = "value2";

  EXPECT_EQ(map[pointer1], "value1");
  EXPECT_EQ(map[pointer2], "value2");
}

TEST(JSONPointer_hash, long_pointer) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  const sourcemeta::core::Pointer pointer1{"a", "b", "c", "d", "e"};
  const sourcemeta::core::Pointer pointer2{"a", "b", "x", "d", "e"};
  const sourcemeta::core::Pointer pointer3{"a", "b", "c", "d", "f"};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 3);
}
