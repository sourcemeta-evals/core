#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <functional> // std::reference_wrapper
#include <string>     // std::string
#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, pointer_empty) {
  const sourcemeta::core::Pointer pointer;
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, pointer_single_property) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, pointer_single_index) {
  const sourcemeta::core::Pointer pointer{0};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, pointer_two_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", 1};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, pointer_multiple_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", 0, "baz"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, pointer_with_hyphen) {
  const sourcemeta::core::Pointer pointer{"foo", "-"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, pointer_different_hashes) {
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const std::hash<sourcemeta::core::Pointer> hasher;
  // Different pointers should (very likely) have different hashes
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_equal_same_hash) {
  const sourcemeta::core::Pointer pointer1{"foo", "bar", 0};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", 0};
  const std::hash<sourcemeta::core::Pointer> hasher;
  // Equal pointers must have equal hashes
  EXPECT_EQ(pointer1, pointer2);
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, pointer_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const sourcemeta::core::Pointer pointer3{"foo"};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  // Should have 2 unique pointers
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(pointer1));
  EXPECT_TRUE(set.contains(pointer2));
  EXPECT_TRUE(set.contains(pointer3));
}

TEST(JSONPointer_hash, pointer_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  const sourcemeta::core::Pointer pointer1{"foo"};
  const sourcemeta::core::Pointer pointer2{"bar"};
  const sourcemeta::core::Pointer pointer3{"baz"};

  map[pointer1] = 1;
  map[pointer2] = 2;
  map[pointer3] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map.at(pointer1), 1);
  EXPECT_EQ(map.at(pointer2), 2);
  EXPECT_EQ(map.at(pointer3), 3);

  // Lookup with equivalent pointer
  const sourcemeta::core::Pointer pointer1_copy{"foo"};
  EXPECT_EQ(map.at(pointer1_copy), 1);
}

TEST(JSONPointer_hash, pointer_unordered_map_complex) {
  std::unordered_map<sourcemeta::core::Pointer, std::string> map;
  const sourcemeta::core::Pointer pointer1{"foo", "bar", 0};
  const sourcemeta::core::Pointer pointer2{"foo", "bar", 1};
  const sourcemeta::core::Pointer pointer3{"foo", "baz", 0};

  map[pointer1] = "first";
  map[pointer2] = "second";
  map[pointer3] = "third";

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map.at(pointer1), "first");
  EXPECT_EQ(map.at(pointer2), "second");
  EXPECT_EQ(map.at(pointer3), "third");
}

TEST(JSONPointer_hash, weak_pointer_empty) {
  const sourcemeta::core::WeakPointer pointer;
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, weak_pointer_single_property) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, weak_pointer_single_index) {
  const sourcemeta::core::WeakPointer pointer{0};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, weak_pointer_two_tokens) {
  const std::string foo = "foo";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), 1};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, weak_pointer_multiple_tokens) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo), std::cref(bar), 0,
                                              std::cref(baz)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, weak_pointer_with_hyphen) {
  const std::string foo = "foo";
  const std::string hyphen = "-";
  const sourcemeta::core::WeakPointer pointer{std::cref(foo),
                                              std::cref(hyphen)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  const auto hash = hasher(pointer);
  // Hash should be deterministic
  EXPECT_EQ(hash, hasher(pointer));
}

TEST(JSONPointer_hash, weak_pointer_different_hashes) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  // Different pointers should (very likely) have different hashes
  EXPECT_NE(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weak_pointer_equal_same_hash) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar),
                                               0};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar),
                                               0};
  const std::hash<sourcemeta::core::WeakPointer> hasher;
  // Equal pointers must have equal hashes
  EXPECT_EQ(pointer1, pointer2);
  EXPECT_EQ(hasher(pointer1), hasher(pointer2));
}

TEST(JSONPointer_hash, weak_pointer_unordered_set) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo)};

  set.insert(pointer1);
  set.insert(pointer2);
  set.insert(pointer3);

  // Should have 2 unique pointers
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.contains(pointer1));
  EXPECT_TRUE(set.contains(pointer2));
  EXPECT_TRUE(set.contains(pointer3));
}

TEST(JSONPointer_hash, weak_pointer_unordered_map) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo)};
  const sourcemeta::core::WeakPointer pointer2{std::cref(bar)};
  const sourcemeta::core::WeakPointer pointer3{std::cref(baz)};

  map[pointer1] = 1;
  map[pointer2] = 2;
  map[pointer3] = 3;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map.at(pointer1), 1);
  EXPECT_EQ(map.at(pointer2), 2);
  EXPECT_EQ(map.at(pointer3), 3);

  // Lookup with equivalent pointer
  const sourcemeta::core::WeakPointer pointer1_copy{std::cref(foo)};
  EXPECT_EQ(map.at(pointer1_copy), 1);
}

TEST(JSONPointer_hash, weak_pointer_unordered_map_complex) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";
  std::unordered_map<sourcemeta::core::WeakPointer, std::string> map;
  const sourcemeta::core::WeakPointer pointer1{std::cref(foo), std::cref(bar),
                                               0};
  const sourcemeta::core::WeakPointer pointer2{std::cref(foo), std::cref(bar),
                                               1};
  const sourcemeta::core::WeakPointer pointer3{std::cref(foo), std::cref(baz),
                                               0};

  map[pointer1] = "first";
  map[pointer2] = "second";
  map[pointer3] = "third";

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map.at(pointer1), "first");
  EXPECT_EQ(map.at(pointer2), "second");
  EXPECT_EQ(map.at(pointer3), "third");
}

TEST(JSONPointer_hash, pointer_and_weak_pointer_consistency) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const sourcemeta::core::Pointer pointer{"foo", "bar", 0};
  const sourcemeta::core::WeakPointer weak_pointer{std::cref(foo),
                                                   std::cref(bar), 0};

  const std::hash<sourcemeta::core::Pointer> pointer_hasher;
  const std::hash<sourcemeta::core::WeakPointer> weak_pointer_hasher;

  // While the hashes may differ (different types), both should be consistent
  // with their equality semantics
  EXPECT_EQ(pointer_hasher(pointer), pointer_hasher(pointer));
  EXPECT_EQ(weak_pointer_hasher(weak_pointer),
            weak_pointer_hasher(weak_pointer));
}
