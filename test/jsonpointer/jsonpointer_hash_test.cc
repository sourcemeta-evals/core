#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <functional>    // std::hash
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set

TEST(JSONPointer_hash, empty_pointer_is_zero) {
  const sourcemeta::core::Pointer pointer;
  EXPECT_EQ(std::hash<sourcemeta::core::Pointer>{}(pointer), 0);
}

TEST(JSONPointer_hash, empty_pointer_consistent) {
  const sourcemeta::core::Pointer pointer;
  EXPECT_EQ(std::hash<sourcemeta::core::Pointer>{}(pointer),
            std::hash<sourcemeta::core::Pointer>{}(pointer));
}

TEST(JSONPointer_hash, equal_pointers_same_hash) {
  const sourcemeta::core::Pointer left{"foo", "bar"};
  const sourcemeta::core::Pointer right{"foo", "bar"};
  EXPECT_EQ(std::hash<sourcemeta::core::Pointer>{}(left),
            std::hash<sourcemeta::core::Pointer>{}(right));
}

TEST(JSONPointer_hash, property_pointer_consistent) {
  const sourcemeta::core::Pointer pointer{"foo"};
  EXPECT_EQ(std::hash<sourcemeta::core::Pointer>{}(pointer),
            std::hash<sourcemeta::core::Pointer>{}(pointer));
}

TEST(JSONPointer_hash, index_pointer_consistent) {
  const sourcemeta::core::Pointer pointer{0};
  EXPECT_EQ(std::hash<sourcemeta::core::Pointer>{}(pointer),
            std::hash<sourcemeta::core::Pointer>{}(pointer));
}

TEST(JSONPointer_hash, different_property_pointers_different_hash) {
  const sourcemeta::core::Pointer left{"foo"};
  const sourcemeta::core::Pointer right{"bar"};
  EXPECT_NE(std::hash<sourcemeta::core::Pointer>{}(left),
            std::hash<sourcemeta::core::Pointer>{}(right));
}

TEST(JSONPointer_hash, different_index_pointers_different_hash) {
  const sourcemeta::core::Pointer left{0};
  const sourcemeta::core::Pointer right{1};
  EXPECT_NE(std::hash<sourcemeta::core::Pointer>{}(left),
            std::hash<sourcemeta::core::Pointer>{}(right));
}

TEST(JSONPointer_hash, unordered_set_pointer) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{"foo", "bar"});
  set.insert(sourcemeta::core::Pointer{"baz"});
  set.insert(sourcemeta::core::Pointer{});
  EXPECT_EQ(set.size(), 3);
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"foo", "bar"}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{"baz"}));
  EXPECT_TRUE(set.contains(sourcemeta::core::Pointer{}));
  EXPECT_FALSE(set.contains(sourcemeta::core::Pointer{"foo"}));
}

TEST(JSONPointer_hash, unordered_map_pointer) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map[sourcemeta::core::Pointer{"foo", "bar"}] = 1;
  map[sourcemeta::core::Pointer{0}] = 2;
  map[sourcemeta::core::Pointer{}] = 3;
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{"foo", "bar"}), 1);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{0}), 2);
  EXPECT_EQ(map.at(sourcemeta::core::Pointer{}), 3);
}

TEST(JSONPointer_hash, unordered_set_no_duplicates) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{"foo"});
  set.insert(sourcemeta::core::Pointer{"foo"});
  EXPECT_EQ(set.size(), 1);
}

TEST(JSONPointer_hash, multi_token_consistent) {
  const sourcemeta::core::Pointer pointer{"foo", "bar", "baz"};
  EXPECT_EQ(std::hash<sourcemeta::core::Pointer>{}(pointer),
            std::hash<sourcemeta::core::Pointer>{}(pointer));
}

TEST(JSONPointer_hash, empty_weak_pointer_is_zero) {
  const sourcemeta::core::WeakPointer pointer;
  EXPECT_EQ(std::hash<sourcemeta::core::WeakPointer>{}(pointer), 0);
}

TEST(JSONPointer_hash, equal_weak_pointers_same_hash) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const sourcemeta::core::WeakPointer left{std::cref(foo), std::cref(bar)};
  const sourcemeta::core::WeakPointer right{std::cref(foo), std::cref(bar)};
  EXPECT_EQ(std::hash<sourcemeta::core::WeakPointer>{}(left),
            std::hash<sourcemeta::core::WeakPointer>{}(right));
}

TEST(JSONPointer_hash, unordered_set_weak_pointer) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};

  std::unordered_set<sourcemeta::core::WeakPointer> set;
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(bar)});
  set.insert(sourcemeta::core::WeakPointer{});
  EXPECT_EQ(set.size(), 3);
  EXPECT_TRUE(set.contains(sourcemeta::core::WeakPointer{std::cref(foo)}));
  EXPECT_TRUE(set.contains(sourcemeta::core::WeakPointer{std::cref(bar)}));
  EXPECT_TRUE(set.contains(sourcemeta::core::WeakPointer{}));
}

TEST(JSONPointer_hash, unordered_map_weak_pointer) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};

  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  map[sourcemeta::core::WeakPointer{std::cref(foo)}] = 1;
  map[sourcemeta::core::WeakPointer{std::cref(bar)}] = 2;
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(foo)}), 1);
  EXPECT_EQ(map.at(sourcemeta::core::WeakPointer{std::cref(bar)}), 2);
}
