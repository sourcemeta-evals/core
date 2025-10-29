#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_empty) {
  const sourcemeta::core::Pointer pointer;
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_EQ(hash, 0);
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

TEST(JSONPointer_hash, pointer_multiple_properties) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_mixed_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, pointer_different_pointers_different_hashes) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1 = hasher(pointer1);
  const auto hash2 = hasher(pointer2);
  // Different pointers should (usually) have different hashes
  // This is not guaranteed but highly likely
  EXPECT_NE(hash1, hash2);
}

TEST(JSONPointer_hash, pointer_same_pointers_same_hashes) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer2{"foo", "bar"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash1 = hasher(pointer1);
  const auto hash2 = hasher(pointer2);
  EXPECT_EQ(hash1, hash2);
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const sourcemeta::core::Pointer pointer3{"foo"};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(pointer1) != set.end());
  EXPECT_TRUE(set.find(pointer2) != set.end());
  EXPECT_TRUE(set.find(pointer3) != set.end());
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};

  map[pointer1] = 1;
  map[pointer2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 1);
  EXPECT_EQ(map[pointer2], 2);
}

TEST(JSONPointer_hash, pointer_unordered_map_update) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer{"foo", "bar"};

  map[pointer] = 1;
  EXPECT_EQ(map[pointer], 1);

  map[pointer] = 2;
  EXPECT_EQ(map[pointer], 2);
  EXPECT_EQ(map.size(), 1);
}

TEST(JSONPointer_hash, weakpointer_empty) {
  const sourcemeta::core::WeakPointer pointer;
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_EQ(hash, 0);
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

TEST(JSONPointer_hash, weakpointer_multiple_properties) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar),
                                              std::cref(baz)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_mixed_tokens) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), 1,
                                              std::cref(bar)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_different_pointers_different_hashes) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash1 = hasher(pointer1);
  const auto hash2 = hasher(pointer2);
  // Different pointers should (usually) have different hashes
  // This is not guaranteed but highly likely
  EXPECT_NE(hash1, hash2);
}

TEST(JSONPointer_hash, weakpointer_same_pointers_same_hashes) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash1 = hasher(pointer1);
  const auto hash2 = hasher(pointer2);
  EXPECT_EQ(hash1, hash2);
}

TEST(JSONPointer_hash, weakpointer_unordered_set) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo)};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(pointer1) != set.end());
  EXPECT_TRUE(set.find(pointer2) != set.end());
  EXPECT_TRUE(set.find(pointer3) != set.end());
}

TEST(JSONPointer_hash, weakpointer_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};

  map[pointer1] = 1;
  map[pointer2] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[pointer1], 1);
  EXPECT_EQ(map[pointer2], 2);
}

TEST(JSONPointer_hash, weakpointer_unordered_map_update) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar)};

  map[pointer] = 1;
  EXPECT_EQ(map[pointer], 1);

  map[pointer] = 2;
  EXPECT_EQ(map[pointer], 2);
  EXPECT_EQ(map.size(), 1);
}

TEST(JSONPointer_hash, pointer_long_path) {
  const sourcemeta::core::Pointer pointer{"a", "b", "c", "d", "e",
                                          "f", "g", "h", "i", "j"};
  std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}

TEST(JSONPointer_hash, weakpointer_long_path) {
  const std::string a = "a";
  const std::string b = "b";
  const std::string c = "c";
  const std::string d = "d";
  const std::string e = "e";
  const std::string f = "f";
  const std::string g = "g";
  const std::string h = "h";
  const std::string i = "i";
  const std::string j = "j";
  const sourcemeta::core::WeakPointer pointer{
      std::cref(a), std::cref(b), std::cref(c), std::cref(d), std::cref(e),
      std::cref(f), std::cref(g), std::cref(h), std::cref(i), std::cref(j)};
  std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  EXPECT_NE(hash, 0);
}
