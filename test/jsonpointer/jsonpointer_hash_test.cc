#include <gtest/gtest.h>
#include <sourcemeta/core/jsonpointer.h>
#include <unordered_map>
#include <unordered_set>

TEST(JSONPointerHash, UnorderedMapPointer) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  sourcemeta::core::Pointer p1{"foo"};
  map[p1] = 1;
  EXPECT_EQ(map[p1], 1);
}

TEST(JSONPointerHash, UnorderedSetPointer) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  sourcemeta::core::Pointer p1{"foo"};
  set.insert(p1);
  EXPECT_TRUE(set.find(p1) != set.end());
}

TEST(JSONPointerHash, UnorderedMapWeakPointer) {
  std::unordered_map<sourcemeta::core::WeakPointer, int> map;
  std::string foo = "foo";
  sourcemeta::core::WeakPointer p1{std::cref(foo)};
  map[p1] = 1;
  EXPECT_EQ(map[p1], 1);
}

TEST(JSONPointerHash, UnorderedSetWeakPointer) {
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  std::string foo = "foo";
  sourcemeta::core::WeakPointer p1{std::cref(foo)};
  set.insert(p1);
  EXPECT_TRUE(set.find(p1) != set.end());
}
