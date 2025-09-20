#include <gtest/gtest.h>

#include <sourcemeta/core/jsonschema.h>

#include <string>
#include <type_traits>
#include <vector>

#include "jsonschema_transform_rules.h"

TEST(JSONSchema_transformer_iterator, empty_bundle) {
  const sourcemeta::core::SchemaTransformer bundle;
  EXPECT_TRUE(bundle.empty());
  EXPECT_EQ(bundle.size(), static_cast<std::size_t>(0));
  EXPECT_EQ(bundle.begin(), bundle.end());
  EXPECT_EQ(bundle.cbegin(), bundle.cend());
}

TEST(JSONSchema_transformer_iterator, iterate_rules_readonly) {
  sourcemeta::core::SchemaTransformer bundle;

  bundle.add<ExampleRule2>();
  bundle.add<ExampleRule1>();

  EXPECT_FALSE(bundle.empty());
  EXPECT_EQ(bundle.size(), static_cast<std::size_t>(2));

  std::vector<std::string> names;
  std::vector<std::string> messages;
  for (const auto &entry : bundle) {
    names.push_back(entry.first);
    messages.push_back(entry.second->message());
  }

  ASSERT_EQ(names.size(), static_cast<std::size_t>(2));
  ASSERT_EQ(messages.size(), static_cast<std::size_t>(2));

  EXPECT_EQ(names.at(0), "example_rule_1");
  EXPECT_EQ(names.at(1), "example_rule_2");
  EXPECT_EQ(messages.at(0), "Keyword foo is not permitted");
  EXPECT_EQ(messages.at(1), "Keyword bar is not permitted");
}

TEST(JSONSchema_transformer_iterator, const_iterator_type) {
  const sourcemeta::core::SchemaTransformer bundle;

  using IteratorT = decltype(bundle.begin());
  static_assert(
      std::is_same_v<IteratorT,
                     sourcemeta::core::SchemaTransformer::const_iterator>,
      "begin() must return const_iterator");

  using CIteratorT = decltype(bundle.cbegin());
  static_assert(
      std::is_same_v<CIteratorT,
                     sourcemeta::core::SchemaTransformer::const_iterator>,
      "cbegin() must return const_iterator");
}

TEST(JSONSchema_transformer_iterator, iterator_consistency) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule3>();

  auto it1 = bundle.begin();
  auto it2 = bundle.cbegin();
  auto end1 = bundle.end();
  auto end2 = bundle.cend();

  EXPECT_EQ(it1, it2);
  EXPECT_EQ(end1, end2);
  EXPECT_NE(it1, end1);
  EXPECT_NE(it2, end2);

  std::size_t count = 0;
  for (auto it = bundle.begin(); it != bundle.end(); ++it) {
    ++count;
  }
  EXPECT_EQ(count, bundle.size());
}
