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
}

TEST(JSONPointer_hash, unordered_set_weakpointer) {
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

TEST(JSONPointer_hash, unordered_map_pointer) {
  std::unordered_map<sourcemeta::core::Pointer, int> pointer_map;

  pointer_map[sourcemeta::core::Pointer{"foo"}] = 1;
  pointer_map[sourcemeta::core::Pointer{"bar"}] = 2;
  pointer_map[sourcemeta::core::Pointer{"baz"}] = 3;

  EXPECT_EQ(pointer_map.size(), 3);
  EXPECT_EQ(pointer_map[sourcemeta::core::Pointer{"foo"}], 1);
  EXPECT_EQ(pointer_map[sourcemeta::core::Pointer{"bar"}], 2);
  EXPECT_EQ(pointer_map[sourcemeta::core::Pointer{"baz"}], 3);
}

TEST(JSONPointer_hash, unordered_map_weakpointer) {
  const std::string foo = "foo";
  const std::string bar = "bar";
  const std::string baz = "baz";

  std::unordered_map<sourcemeta::core::WeakPointer, int> pointer_map;

  pointer_map[sourcemeta::core::WeakPointer{std::cref(foo)}] = 1;
  pointer_map[sourcemeta::core::WeakPointer{std::cref(bar)}] = 2;
  pointer_map[sourcemeta::core::WeakPointer{std::cref(baz)}] = 3;

  EXPECT_EQ(pointer_map.size(), 3);
  EXPECT_EQ(pointer_map[sourcemeta::core::WeakPointer{std::cref(foo)}], 1);
  EXPECT_EQ(pointer_map[sourcemeta::core::WeakPointer{std::cref(bar)}], 2);
  EXPECT_EQ(pointer_map[sourcemeta::core::WeakPointer{std::cref(baz)}], 3);
}

TEST(JSONPointer_hash, empty_pointer) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;
  pointers.insert(sourcemeta::core::Pointer{});

  EXPECT_EQ(pointers.size(), 1);
  EXPECT_TRUE(pointers.contains(sourcemeta::core::Pointer{}));
}

TEST(JSONPointer_hash, mixed_property_and_index) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;

  pointers.insert(sourcemeta::core::Pointer{"foo", 1, "bar"});
  pointers.insert(sourcemeta::core::Pointer{"foo", 2, "bar"});
  pointers.insert(sourcemeta::core::Pointer{"baz", 1, "qux"});

  EXPECT_EQ(pointers.size(), 3);
  EXPECT_TRUE(pointers.contains(sourcemeta::core::Pointer{"foo", 1, "bar"}));
  EXPECT_TRUE(pointers.contains(sourcemeta::core::Pointer{"foo", 2, "bar"}));
  EXPECT_TRUE(pointers.contains(sourcemeta::core::Pointer{"baz", 1, "qux"}));
}

TEST(JSONPointer_hash, long_pointer) {
  std::unordered_set<sourcemeta::core::Pointer> pointers;

  pointers.insert(sourcemeta::core::Pointer{"a", "b", "c", "d", "e", "f"});
  pointers.insert(sourcemeta::core::Pointer{"a", "b", "x", "d", "e", "f"});
  pointers.insert(sourcemeta::core::Pointer{"x", "b", "c", "d", "e", "f"});

  EXPECT_EQ(pointers.size(), 3);
  EXPECT_TRUE(pointers.contains(
      sourcemeta::core::Pointer{"a", "b", "c", "d", "e", "f"}));
}
