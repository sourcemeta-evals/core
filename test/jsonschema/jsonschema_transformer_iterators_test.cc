#include <gtest/gtest.h>

#include <sourcemeta/core/jsonschema.h>
#include <string>
#include <utility>
#include <vector>

#include "jsonschema_transform_rules.h"

TEST(JSONSchema_transformer_iterators, empty_bundle_iteration) {
  const sourcemeta::core::SchemaTransformer bundle;
  EXPECT_TRUE(bundle.empty());
  EXPECT_EQ(bundle.size(), static_cast<std::size_t>(0));
  EXPECT_EQ(bundle.begin(), bundle.end());
  EXPECT_EQ(bundle.cbegin(), bundle.cend());
}

TEST(JSONSchema_transformer_iterators, iterate_names_in_order) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule2>();
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule4>();

  std::vector<std::string> observed;
  for (const auto &[name, rule] : bundle) {
    observed.push_back(name);
    ASSERT_NE(rule, nullptr);
    EXPECT_EQ(rule->name(), name);
  }

  const std::vector<std::string> expected{"example_rule_1", "example_rule_2",
                                          "example_rule_4"};
  EXPECT_EQ(observed, expected);
}

TEST(JSONSchema_transformer_iterators, const_iteration) {
  sourcemeta::core::SchemaTransformer mutable_bundle;
  mutable_bundle.add<ExampleRule1>();
  mutable_bundle.add<ExampleRule2>();

  const sourcemeta::core::SchemaTransformer &bundle = mutable_bundle;
  std::vector<std::string> messages;
  for (auto it = bundle.cbegin(); it != bundle.cend(); ++it) {
    const auto &name = it->first;
    const auto &rule = it->second;
    ASSERT_NE(rule, nullptr);
    (void)name;
    messages.push_back(rule->message());
  }

  ASSERT_EQ(messages.size(), static_cast<std::size_t>(2));
  EXPECT_EQ(messages[0], "Keyword foo is not permitted");
  EXPECT_EQ(messages[1], "Keyword bar is not permitted");
}

TEST(JSONSchema_transformer_iterators, size_and_empty_after_adding_rules) {
  sourcemeta::core::SchemaTransformer bundle;
  EXPECT_TRUE(bundle.empty());
  EXPECT_EQ(bundle.size(), static_cast<std::size_t>(0));

  bundle.add<ExampleRule1>();
  EXPECT_FALSE(bundle.empty());
  EXPECT_EQ(bundle.size(), static_cast<std::size_t>(1));

  bundle.add<ExampleRule2>();
  EXPECT_FALSE(bundle.empty());
  EXPECT_EQ(bundle.size(), static_cast<std::size_t>(2));
}
