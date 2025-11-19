#include <functional>
#include <gtest/gtest.h>
#include <sourcemeta/core/jsonpointer.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

TEST(PointerHash, UnorderedSet) {
  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(sourcemeta::core::Pointer{"foo"});
  set.insert(sourcemeta::core::Pointer{"bar", 1});

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(sourcemeta::core::Pointer{"foo"}) != set.end());
  EXPECT_TRUE(set.find(sourcemeta::core::Pointer{"bar", 1}) != set.end());
  EXPECT_TRUE(set.find(sourcemeta::core::Pointer{"baz"}) == set.end());
}

TEST(PointerHash, UnorderedMap) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;
  map[sourcemeta::core::Pointer{"foo"}] = 1;
  map[sourcemeta::core::Pointer{"bar", 1}] = 2;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[sourcemeta::core::Pointer{"foo"}], 1);
  EXPECT_EQ(map[sourcemeta::core::Pointer{"bar", 1}], 2);
}

TEST(WeakPointerHash, UnorderedSet) {
  std::string foo = "foo";
  std::string bar = "bar";
  std::unordered_set<sourcemeta::core::WeakPointer> set;
  set.insert(sourcemeta::core::WeakPointer{std::cref(foo)});
  set.insert(sourcemeta::core::WeakPointer{std::cref(bar), 1});

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(sourcemeta::core::WeakPointer{std::cref(foo)}) !=
              set.end());
  EXPECT_TRUE(set.find(sourcemeta::core::WeakPointer{std::cref(bar), 1}) !=
              set.end());
}

TEST(PointerHash, CollisionCheck) {
  // Test that different pointers produce different hashes (probabilistically)
  // or at least that they can coexist in the container.
  // The hash function samples first, last, and middle.
  // Let's construct pointers that differ in non-sampled positions.
  // Size 4: 0, 1, 2, 3. Sampled: 0 (first), 3 (last), 2 (middle: 4/2=2).
  // So index 1 is not sampled.

  sourcemeta::core::Pointer p1{"a", "b", "c", "d"};
  sourcemeta::core::Pointer p2{"a", "x", "c", "d"};

  // Hashes might be equal because "b" and "x" are at index 1 which is not
  // sampled. But the container should handle it.

  std::unordered_set<sourcemeta::core::Pointer> set;
  set.insert(p1);
  set.insert(p2);

  EXPECT_EQ(set.size(), 2);
  EXPECT_TRUE(set.find(p1) != set.end());
  EXPECT_TRUE(set.find(p2) != set.end());
}
