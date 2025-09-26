#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <algorithm>
#include <string>
#include <vector>

#include "jsonschema_transform_rules.h"

TEST(JSONSchema_transformer_iterator, empty_transformer) {
  sourcemeta::core::SchemaTransformer transformer;

  EXPECT_EQ(transformer.begin(), transformer.end());
  EXPECT_EQ(transformer.cbegin(), transformer.cend());

  std::vector<std::string> rule_names;
  for (const auto &[name, rule] : transformer) {
    rule_names.push_back(name);
  }
  EXPECT_TRUE(rule_names.empty());
}

TEST(JSONSchema_transformer_iterator, single_rule) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<ExampleRule1>();

  EXPECT_NE(transformer.begin(), transformer.end());
  EXPECT_NE(transformer.cbegin(), transformer.cend());

  std::vector<std::string> rule_names;
  for (const auto &[name, rule] : transformer) {
    rule_names.push_back(name);
    EXPECT_EQ(rule->name(), name);
    EXPECT_EQ(rule->message(), "Keyword foo is not permitted");
  }

  EXPECT_EQ(rule_names.size(), 1);
  EXPECT_EQ(rule_names[0], "example_rule_1");
}

TEST(JSONSchema_transformer_iterator, multiple_rules) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<ExampleRule1>();
  transformer.add<ExampleRule2>();
  transformer.add<ExampleRule3>();

  std::vector<std::string> rule_names;
  for (const auto &[name, rule] : transformer) {
    rule_names.push_back(name);
    EXPECT_EQ(rule->name(), name);
  }

  EXPECT_EQ(rule_names.size(), 3);

  std::sort(rule_names.begin(), rule_names.end());
  EXPECT_EQ(rule_names[0], "example_rule_1");
  EXPECT_EQ(rule_names[1], "example_rule_2");
  EXPECT_EQ(rule_names[2], "example_rule_3");
}

TEST(JSONSchema_transformer_iterator, const_iterator_methods) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<ExampleRule1>();
  transformer.add<ExampleRule2>();

  auto it = transformer.cbegin();
  auto end = transformer.cend();

  std::vector<std::string> rule_names;
  while (it != end) {
    rule_names.push_back(it->first);
    EXPECT_EQ(it->second->name(), it->first);
    ++it;
  }

  EXPECT_EQ(rule_names.size(), 2);
}

TEST(JSONSchema_transformer_iterator, iterator_consistency) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<ExampleRule1>();
  transformer.add<ExampleRule2>();

  std::vector<std::string> names1, names2;

  for (auto it = transformer.begin(); it != transformer.end(); ++it) {
    names1.push_back(it->first);
  }

  for (auto it = transformer.cbegin(); it != transformer.cend(); ++it) {
    names2.push_back(it->first);
  }

  EXPECT_EQ(names1, names2);
}

TEST(JSONSchema_transformer_iterator, rule_removal_affects_iteration) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<ExampleRule1>();
  transformer.add<ExampleRule2>();

  std::size_t count_before = 0;
  for (const auto &[name, rule] : transformer) {
    ++count_before;
  }
  EXPECT_EQ(count_before, 2);

  EXPECT_TRUE(transformer.remove("example_rule_1"));

  std::size_t count_after = 0;
  std::vector<std::string> remaining_names;
  for (const auto &[name, rule] : transformer) {
    ++count_after;
    remaining_names.push_back(name);
  }

  EXPECT_EQ(count_after, 1);
  EXPECT_EQ(remaining_names.size(), 1);
  EXPECT_EQ(remaining_names[0], "example_rule_2");
}

TEST(JSONSchema_transformer_iterator, rule_introspection) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<ExampleRule1>();
  transformer.add<ExampleRule3>();

  std::vector<std::pair<std::string, std::string>> rule_info;
  for (const auto &[name, rule] : transformer) {
    rule_info.emplace_back(name, rule->message());
  }

  EXPECT_EQ(rule_info.size(), 2);

  std::sort(rule_info.begin(), rule_info.end());
  EXPECT_EQ(rule_info[0].first, "example_rule_1");
  EXPECT_EQ(rule_info[0].second, "Keyword foo is not permitted");
  EXPECT_EQ(rule_info[1].first, "example_rule_3");
  EXPECT_EQ(rule_info[1].second, "Example rule 3");
}
