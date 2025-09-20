#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "jsonschema_transform_rules.h"
#include <sourcemeta/core/jsonschema.h>

TEST(JSONSchema_transformer_iterators, empty_bundle) {
  sourcemeta::core::SchemaTransformer bundle;
  EXPECT_EQ(bundle.cbegin(), bundle.cend());
  EXPECT_EQ(bundle.begin(), bundle.end());
}

TEST(JSONSchema_transformer_iterators, iterate_names) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule2>();

  std::vector<std::string> names;
  for (auto it = bundle.cbegin(); it != bundle.cend(); ++it) {
    names.emplace_back(it->first);
  }

  ASSERT_EQ(names.size(), 2U);
  EXPECT_EQ(names[0], "example_rule_1");
  EXPECT_EQ(names[1], "example_rule_2");
}

TEST(JSONSchema_transformer_iterators, range_based_loop) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule2>();

  std::vector<std::string> names;
  for (const auto &entry : bundle) {
    names.emplace_back(entry.first);
  }

  ASSERT_EQ(names.size(), 2U);
  EXPECT_EQ(names[0], "example_rule_1");
  EXPECT_EQ(names[1], "example_rule_2");
}
