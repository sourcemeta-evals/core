#include <gtest/gtest.h>
#include <sourcemeta/core/jsonpointer.h>
#include <unordered_map>
#include <unordered_set>

TEST(JSONPointerHash, UnorderedSet) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{"foo"});
  set.insert(sourcemeta::core::Pointer{"bar"});

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(sourcemeta::core::Pointer{"foo"}) != set.end());
  EXPECT_TRUE(set.find(sourcemeta::core::Pointer{"baz"}) == set.end());
}

TEST(JSONPointerHash, UnorderedMap) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map[sourcemeta::core::Pointer{"foo"}] = 1;
  map[sourcemeta::core::Pointer{"bar"}] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[sourcemeta::core::Pointer{"foo"}], 1);
  EXPECT_EQ(map[sourcemeta::core::Pointer{"bar"}], 2);
}

TEST(JSONPointerHash, WeakPointerUnorderedSet) {
  std::string foo = "foo";
  std::string bar = "bar";
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(bar)});

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(sourcemeta::core::WeakPointer{std::cref(foo)}) !=
              set.end());
}
