#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <string>
#include <unordered_map>
#include <unordered_set>

using sourcemeta::core::Pointer;
using sourcemeta::core::WeakPointer;

TEST(JSONPointerHash, unordered_set_pointer_basic_dedup) {
  std::unordered_set<Pointer> set;
  set.insert(Pointer{});
  set.insert(Pointer{});
  EXPECT_EQ(set.size(), 1);

  set.insert(Pointer{"foo"});
  set.insert(Pointer{"foo"});
  EXPECT_EQ(set.count(Pointer{"foo"}), 1U);
}

TEST(JSONPointerHash, unordered_map_pointer_basic_get_set) {
  std::unordered_map<Pointer, int> map;
  map[Pointer{}] = 1;
  map[Pointer{"foo"}] = 2;
  map[Pointer{1}] = 3;
  map[Pointer{"foo", 1}] = 4;
  map[Pointer{"-"}] = 5;

  EXPECT_EQ(map.at(Pointer{}), 1);
  EXPECT_EQ(map.at(Pointer{"foo"}), 2);
  EXPECT_EQ(map.at(Pointer{1}), 3);
  EXPECT_EQ(map.at(Pointer{"foo", 1}), 4);
  EXPECT_EQ(map.at(Pointer{"-"}), 5);
}

TEST(JSONPointerHash, unordered_set_weakpointer_basic_dedup) {
  const std::string foo{"foo"};
  WeakPointer p1{std::cref(foo)};
  WeakPointer p2{std::cref(foo)};

  std::unordered_set<WeakPointer> set;
  set.insert(p1);
  set.insert(p2);
  EXPECT_EQ(set.size(), 1);
}

TEST(JSONPointerHash, unordered_map_weakpointer_basic_get_set) {
  const std::string foo{"foo"};
  const std::string bar{"bar"};
  const std::string hyphen{"-"};

  std::unordered_map<WeakPointer, int> map;
  map[WeakPointer{}] = 1;
  map[WeakPointer{std::cref(foo)}] = 2;
  map[WeakPointer{1}] = 3;
  map[WeakPointer{std::cref(foo), 1, std::cref(bar)}] = 4;
  map[WeakPointer{std::cref(hyphen)}] = 5;

  EXPECT_EQ(map.at(WeakPointer{}), 1);
  EXPECT_EQ(map.at(WeakPointer{std::cref(foo)}), 2);
  EXPECT_EQ(map.at(WeakPointer{1}), 3);
  EXPECT_EQ(map.at(WeakPointer{std::cref(foo), 1, std::cref(bar)}), 4);
  EXPECT_EQ(map.at(WeakPointer{std::cref(hyphen)}), 5);
}
