#include <gtest/gtest.h>

#include <sourcemeta/core/jsonpointer.h>

#include <unordered_map>
#include <unordered_set>

TEST(JSONPointer_hash, unordered_set_pointer) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;

  pointers.insert(sourcemeta::core::Pointer{"foo"});
  pointers.insert(sourcemeta::core::Pointer{"bar"});
  pointers.insert(sourcemeta::core::Pointer{"baz"});
  pointers.insert(sourcemeta::core::Pointer{"foo"});

  EXPECT_EQ(pointers.size(), 3);
  EXPECT_TRUE(pointers.contains(sourcemeta::core::Pointer{"foo"}));
  EXPECT_TRUE(pointers.contains(sourcemeta::core::Pointer{"bar"}));
  EXPECT_TRUE(pointers.contains(sourcemeta::core::Pointer{"baz"}));
  EXPECT_FALSE(pointers.contains(sourcemeta::core::Pointer{"qux"}));
}

TEST(JSONPointer_hash, unordered_set_pointer_mixed_tokens) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;

  pointers.insert(sourcemeta::core::Pointer{"foo", 1, "bar"});
  pointers.insert(sourcemeta::core::Pointer{"foo", 2, "bar"});
  pointers.insert(sourcemeta::core::Pointer{"foo", 1, "bar"});

  EXPECT_EQ(pointers.size(), 2);
  EXPECT_TRUE(pointers.contains(sourcemeta::core::Pointer{"foo", 1, "bar"}));
  EXPECT_TRUE(pointers.contains(sourcemeta::core::Pointer{"foo", 2, "bar"}));
  EXPECT_FALSE(pointers.contains(sourcemeta::core::Pointer{"foo", 3, "bar"}));
}

TEST(JSONPointer_hash, unordered_set_pointer_empty) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;

  pointers.insert(sourcemeta::core::Pointer{});
  pointers.insert(sourcemeta::core::Pointer{});

  EXPECT_EQ(pointers.size(), 1);
  EXPECT_TRUE(pointers.contains(sourcemeta::core::Pointer{}));
}

TEST(JSONPointer_hash, unordered_map_pointer) {
  std::unordered_map<sourcemeta::core::Pointer, int> map;

  map[sourcemeta::core::Pointer{"foo"}] = 1;
  map[sourcemeta::core::Pointer{"bar"}] = 2;
  map[sourcemeta::core::Pointer{"baz"}] = 3;
  map[sourcemeta::core::Pointer{"foo"}] = 10;

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(map[sourcemeta::core::Pointer{"foo"}], 10);
  EXPECT_EQ(map[sourcemeta::core::Pointer{"bar"}], 2);
  EXPECT_EQ(map[sourcemeta::core::Pointer{"baz"}], 3);
}

TEST(JSONPointer_hash, unordered_map_pointer_complex) {
  std::unordered_map<sourcemeta::core::Pointer, std::string> map;

  map[sourcemeta::core::Pointer{"foo", "bar", "baz"}] = "value1";
  map[sourcemeta::core::Pointer{"foo", 0, "bar"}] = "value2";
  map[sourcemeta::core::Pointer{0, 1, 2}] = "value3";

  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ((map[sourcemeta::core::Pointer{"foo", "bar", "baz"}]), "value1");
  EXPECT_EQ((map[sourcemeta::core::Pointer{"foo", 0, "bar"}]), "value2");
  EXPECT_EQ((map[sourcemeta::core::Pointer{0, 1, 2}]), "value3");
}

TEST(JSONPointer_hash, unordered_set_weak_pointer) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  std::unordered_set<sourcemeta::core::WeakPointer> pointers;

  pointers.insert(sourcemeta::core::WeakPointer{std::cref(foo)});
  pointers.insert(sourcemeta::core::WeakPointer{std::cref(bar)});
  pointers.insert(sourcemeta::core::WeakPointer{std::cref(baz)});
  pointers.insert(sourcemeta::core::WeakPointer{std::cref(foo)});

  EXPECT_EQ(pointers.size(), 3);
  EXPECT_TRUE(pointers.contains(sourcemeta::core::WeakPointer{std::cref(foo)}));
  EXPECT_TRUE(pointers.contains(sourcemeta::core::WeakPointer{std::cref(bar)}));
  EXPECT_TRUE(pointers.contains(sourcemeta::core::WeakPointer{std::cref(baz)}));
}

TEST(JSONPointer_hash, unordered_map_weak_pointer) {
  const std::string foo = "foo";
  const std::string bar = "bar";

  std::unordered_map<sourcemeta::core::WeakPointer, int> map;

  map[sourcemeta::core::WeakPointer{std::cref(foo)}] = 1;
  map[sourcemeta::core::WeakPointer{std::cref(bar)}] = 2;
  map[sourcemeta::core::WeakPointer{std::cref(foo)}] = 10;

  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map[sourcemeta::core::WeakPointer{std::cref(foo)}], 10);
  EXPECT_EQ(map[sourcemeta::core::WeakPointer{std::cref(bar)}], 2);
}
