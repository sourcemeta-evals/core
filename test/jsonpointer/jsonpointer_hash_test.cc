#include <gtest/gtest.h>
#include <sourcemeta/core/jsonpointer.h>
#include <unordered_map>
#include <unordered_set>

using namespace sourcemeta::core;

TEST(JSONPointerHashTest, UnorderedSetPointer) {
  std::unordered_set<Pointer> set;
  set.insert(Pointer{"foo"});
  set.insert(Pointer{"bar"});
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(Pointer{"foo"}) != set.end());
}

TEST(JSONPointerHashTest, UnorderedMapPointer) {
  std::unordered_map<Pointer, int> map;
  map[Pointer{"foo"}] = 1;
  map[Pointer{"bar"}] = 2;
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[Pointer{"foo"}], 1);
}

TEST(JSONPointerHashTest, UnorderedSetWeakPointer) {
  std::string foo = "foo";
  std::string bar = "bar";
  std::unordered_set<WeakPointer> set;
  set.insert(WeakPointer{std::cref(foo)});
  set.insert(WeakPointer{std::cref(bar)});
  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(WeakPointer{std::cref(foo)}) != set.end());
}
