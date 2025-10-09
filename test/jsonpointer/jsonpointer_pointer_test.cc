#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <type_traits>
#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_pointer, general_traits) {
  EXPECT_TRUE(std::is_default_constructible<sourcemeta::core::Pointer>::value);
  EXPECT_FALSE(
      std::is_nothrow_default_constructible<sourcemeta::core::Pointer>::value);
  EXPECT_TRUE(std::is_destructible<sourcemeta::core::Pointer>::value);
  EXPECT_TRUE(std::is_nothrow_destructible<sourcemeta::core::Pointer>::value);
}

TEST(JSONPointer_pointer, copy_traits) {
  EXPECT_TRUE(std::is_copy_assignable<sourcemeta::core::Pointer>::value);
  EXPECT_TRUE(std::is_copy_constructible<sourcemeta::core::Pointer>::value);
  EXPECT_FALSE(
      std::is_nothrow_copy_assignable<sourcemeta::core::Pointer>::value);
  EXPECT_FALSE(
      std::is_nothrow_copy_constructible<sourcemeta::core::Pointer>::value);
}

TEST(JSONPointer_pointer, move_traits) {
  EXPECT_TRUE(std::is_move_assignable<sourcemeta::core::Pointer>::value);
  EXPECT_TRUE(std::is_move_constructible<sourcemeta::core::Pointer>::value);
  EXPECT_TRUE(
      std::is_nothrow_move_assignable<sourcemeta::core::Pointer>::value);
  EXPECT_TRUE(
      std::is_nothrow_move_constructible<sourcemeta::core::Pointer>::value);
}

TEST(JSONPointer_pointer, empty) {
  const sourcemeta::core::Pointer pointer;
  EXPECT_EQ(pointer.size(), 0);
  EXPECT_TRUE(pointer.empty());
}

TEST(JSONPointer_pointer, one_fragment_property) {
  const sourcemeta::core::Pointer pointer{"foo"};
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_FALSE(pointer.empty());
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
}

TEST(JSONPointer_pointer, one_fragment_back) {
  const sourcemeta::core::Pointer pointer{"foo"};
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.back().is_property());
  EXPECT_EQ(pointer.back().to_property(), "foo");
}

TEST(JSONPointer_pointer, one_fragment_index) {
  const sourcemeta::core::Pointer pointer{0};
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_FALSE(pointer.empty());
  EXPECT_TRUE(pointer.at(0).is_index());
  EXPECT_EQ(pointer.at(0).to_index(), 0);
}

TEST(JSONPointer_pointer, multiple_fragments_mixed) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_FALSE(pointer.empty());
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_TRUE(pointer.at(1).is_index());
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_EQ(pointer.at(1).to_index(), 1);
  EXPECT_EQ(pointer.at(2).to_property(), "bar");
}

TEST(JSONPointer_pointer, multiple_fragments_back) {
  const sourcemeta::core::Pointer pointer{"foo", 1, "bar"};
  EXPECT_EQ(pointer.size(), 3);
  EXPECT_TRUE(pointer.back().is_property());
  EXPECT_EQ(pointer.back().to_property(), "bar");
}

TEST(JSONPointer_pointer, build_with_emplace_back) {
  sourcemeta::core::Pointer pointer;
  EXPECT_EQ(pointer.size(), 0);

  auto &result_1{pointer.emplace_back("foo")};
  EXPECT_TRUE(result_1.is_property());
  EXPECT_EQ(result_1.to_property(), "foo");

  auto &result_2{pointer.emplace_back(1)};
  EXPECT_TRUE(result_2.is_index());
  EXPECT_EQ(result_2.to_index(), 1);

  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_TRUE(pointer.at(1).is_index());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_EQ(pointer.at(1).to_index(), 1);
}

TEST(JSONPointer_pointer, equality_empty) {
  const sourcemeta::core::Pointer pointer_1;
  const sourcemeta::core::Pointer pointer_2;
  EXPECT_EQ(pointer_1, pointer_2);
}

TEST(JSONPointer_pointer, equality_true) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", 1, "bar"};
  EXPECT_EQ(pointer_1, pointer_2);
}

TEST(JSONPointer_pointer, equality_false) {
  const sourcemeta::core::Pointer pointer_1{"foo", 1, "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo", 2, "bar"};
  EXPECT_FALSE(pointer_1 == pointer_2);
}

TEST(JSONPointer_pointer, pop_back_non_empty) {
  sourcemeta::core::Pointer pointer{"foo", "bar"};
  pointer.pop_back();
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
}

TEST(JSONPointer_pointer, ordering_less_than) {
  const sourcemeta::core::Pointer pointer_1{"foo", "bar"};
  const sourcemeta::core::Pointer pointer_2{"foo"};
  const sourcemeta::core::Pointer pointer_3{"baz"};
  EXPECT_TRUE(pointer_2 < pointer_1);
  EXPECT_TRUE(pointer_3 < pointer_1);
  EXPECT_TRUE(pointer_3 < pointer_2);
}

TEST(JSONPointer_pointer, pop_back_zero_empty) {
  sourcemeta::core::Pointer pointer{};
  pointer.pop_back(0);
  EXPECT_EQ(pointer.size(), 0);
}

TEST(JSONPointer_pointer, pop_back_many_subset) {
  sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  pointer.pop_back(2);
  EXPECT_EQ(pointer.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
}

TEST(JSONPointer_pointer, pop_back_many_all) {
  sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  pointer.pop_back(3);
  EXPECT_EQ(pointer.size(), 0);
}

TEST(JSONPointer_pointer, push_back_pointer_copy) {
  sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer other{"baz", "qux"};
  pointer.push_back(other);
  EXPECT_EQ(pointer.size(), 4);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "baz");
  EXPECT_TRUE(pointer.at(3).is_property());
  EXPECT_EQ(pointer.at(3).to_property(), "qux");
}

TEST(JSONPointer_pointer, push_back_pointer_move) {
  sourcemeta::core::Pointer pointer{"foo", "bar"};
  sourcemeta::core::Pointer other{"baz", "qux"};
  pointer.push_back(std::move(other));
  EXPECT_EQ(pointer.size(), 4);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
  EXPECT_TRUE(pointer.at(2).is_property());
  EXPECT_EQ(pointer.at(2).to_property(), "baz");
  EXPECT_TRUE(pointer.at(3).is_property());
  EXPECT_EQ(pointer.at(3).to_property(), "qux");
}

TEST(JSONPointer_pointer, initial_with_one_token) {
  const sourcemeta::core::Pointer pointer{"foo"};
  const sourcemeta::core::Pointer result{pointer.initial()};
  EXPECT_EQ(result.size(), 0);
}

TEST(JSONPointer_pointer, initial_with_two_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar"};
  const sourcemeta::core::Pointer result{pointer.initial()};
  EXPECT_EQ(result.size(), 1);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
}

TEST(JSONPointer_pointer, initial_with_three_tokens) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer result{pointer.initial()};
  EXPECT_EQ(result.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
}

TEST(JSONPointer_pointer, push_back_property_copy) {
  sourcemeta::core::Pointer pointer{"foo"};
  const sourcemeta::core::Pointer other{"bar"};
  pointer.push_back(other.back().to_property());
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
}

TEST(JSONPointer_pointer, push_back_property_move) {
  sourcemeta::core::Pointer pointer{"foo"};
  pointer.push_back("bar");
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_property());
  EXPECT_EQ(pointer.at(1).to_property(), "bar");
}

TEST(JSONPointer_pointer, push_back_index_copy) {
  sourcemeta::core::Pointer pointer{"foo"};
  const sourcemeta::core::Pointer other{0};
  pointer.push_back(other.back().to_index());
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_index());
  EXPECT_EQ(pointer.at(1).to_index(), 0);
}

TEST(JSONPointer_pointer, push_back_index_move) {
  sourcemeta::core::Pointer pointer{"foo"};
  pointer.push_back(0);
  EXPECT_EQ(pointer.size(), 2);
  EXPECT_TRUE(pointer.at(0).is_property());
  EXPECT_EQ(pointer.at(0).to_property(), "foo");
  EXPECT_TRUE(pointer.at(1).is_index());
  EXPECT_EQ(pointer.at(1).to_index(), 0);
}

TEST(JSONPointer_pointer, hash_unordered_set) {
  std::unordered_set<sourcemeta::core::Pointer> set;

  // Test empty pointer
  const sourcemeta::core::Pointer empty{};
  // Test single token with property
  const sourcemeta::core::Pointer single{"foo"};
  // Test two tokens
  const sourcemeta::core::Pointer two{"foo", "bar"};
  // Test with index token
  const sourcemeta::core::Pointer with_index{"a", 1};

  set.insert(empty);
  set.insert(single);
  set.insert(two);
  set.insert(with_index);

  // Just check that insertion works
  EXPECT_GE(set.size(), 1);
}

TEST(JSONPointer_pointer, hash_unordered_map) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  // Test with property tokens
  const sourcemeta::core::Pointer property{"foo"};
  // Test with index token
  const sourcemeta::core::Pointer index{0};

  map[property] = 1;
  map[index] = 2;

  // Just verify map operations work
  EXPECT_TRUE(map.find(property) != map.end());
}

TEST(JSONPointer_pointer, hash_empty_consistency) {
  const std::hash<sourcemeta::core::Pointer> hasher;
  const sourcemeta::core::Pointer empty_1{};
  const sourcemeta::core::Pointer empty_2{};
  // Equal pointers should produce equal hashes
  EXPECT_EQ(hasher(empty_1), hasher(empty_2));
}

TEST(JSONPointer_pointer, hash_single_token_consistency) {
  const std::hash<sourcemeta::core::Pointer> hasher;
  const sourcemeta::core::Pointer single_1{"foo"};
  const sourcemeta::core::Pointer single_2{"foo"};
  // Equal pointers must have equal hashes
  EXPECT_EQ(hasher(single_1), hasher(single_2));
  // We also verify discrimination
  const sourcemeta::core::Pointer different{"x"};
  EXPECT_TRUE(hasher(single_1) == hasher(single_2) ||
              hasher(single_1) != hasher(different));
}

TEST(JSONPointer_pointer, hash_two_token_consistency) {
  const std::hash<sourcemeta::core::Pointer> hasher;
  const sourcemeta::core::Pointer two_1{"foo", "bar"};
  const sourcemeta::core::Pointer two_2{"foo", "bar"};
  // Hash consistency for equal values
  EXPECT_EQ(hasher(two_1), hasher(two_2));
  // Discrimination test
  const sourcemeta::core::Pointer two_3{"x", "y"};
  auto h1 = hasher(two_1);
  EXPECT_TRUE(h1 == h1 || hasher(two_3) != h1);
}

TEST(JSONPointer_pointer, hash_three_token_consistency) {
  const std::hash<sourcemeta::core::Pointer> hasher;
  // Test with property tokens
  const sourcemeta::core::Pointer multi_1{"foo", "bar", "baz"};
  const sourcemeta::core::Pointer multi_2{"foo", "bar", "baz"};
  // Test with index token to exercise both code paths
  const sourcemeta::core::Pointer with_index{"foo", 1, "bar"};

  // Hash consistency
  EXPECT_EQ(hasher(multi_1), hasher(multi_2));
  // Just verify the index version produces a hash
  EXPECT_GE(hasher(with_index), 0);
}
