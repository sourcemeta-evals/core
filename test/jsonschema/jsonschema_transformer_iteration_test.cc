#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

#include <sourcemeta/core/jsonschema.h>

#include "jsonschema_transform_rules.h"

TEST(JSONSchema_transformer_iteration, iterate_rules_read_only_range_for) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleUnfixable1>();
  bundle.add<ExampleRuleRemoveIdentifiers>();

  std::vector<std::string> names;
  for (const auto &entry : bundle) {
    names.push_back(entry.first);
  }

  const std::vector<std::string> expected{"example_rule_remove_identifiers",
                                          "example_rule_unfixable_1"};
  EXPECT_EQ(names, expected);
}

TEST(JSONSchema_transformer_iteration, iterate_rules_with_explicit_iterators) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleUnfixable1>();
  bundle.add<ExampleRuleRemoveIdentifiers>();

  const auto begin = bundle.begin();
  const auto end = bundle.end();
  EXPECT_EQ(std::distance(begin, end), 2);

  EXPECT_EQ(std::distance(bundle.cbegin(), bundle.cend()), 2);
}

TEST(JSONSchema_transformer_iteration, iterate_empty_bundle) {
  const sourcemeta::core::SchemaTransformer bundle;

  EXPECT_EQ(bundle.begin(), bundle.end());
  EXPECT_EQ(bundle.cbegin(), bundle.cend());
  EXPECT_EQ(std::distance(bundle.begin(), bundle.end()), 0);

  std::vector<std::string> names;
  for (const auto &entry : bundle) {
    names.push_back(entry.first);
  }
  EXPECT_TRUE(names.empty());
}

TEST(JSONSchema_transformer_iteration, iterate_single_rule) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleRemoveIdentifiers>();

  std::vector<std::string> names;
  for (const auto &entry : bundle) {
    names.push_back(entry.first);
  }

  const std::vector<std::string> expected{"example_rule_remove_identifiers"};
  EXPECT_EQ(names, expected);
}

TEST(JSONSchema_transformer_iteration, iterate_rules_access_rule_properties) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRuleUnfixable1>();
  bundle.add<ExampleRuleRemoveIdentifiers>();

  std::vector<std::pair<std::string, std::string>> rule_info;
  for (const auto &entry : bundle) {
    rule_info.emplace_back(entry.first, entry.second->message());
  }

  std::sort(rule_info.begin(), rule_info.end());

  const std::vector<std::pair<std::string, std::string>> expected{
      {"example_rule_remove_identifiers", "Remove all identifiers"},
      {"example_rule_unfixable_1", "An example rule that cannot be fixed"}};
  EXPECT_EQ(rule_info, expected);
}
