#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <set>
#include <string>

#include "jsonschema_transform_rules.h"

TEST(JSONSchema_transformer_iterator, empty_transformer_iteration) {
  sourcemeta::core::SchemaTransformer transformer;

  std::set<std::string> rule_names;
  for (const auto &entry : transformer) {
    rule_names.insert(entry.first);
  }

  EXPECT_TRUE(rule_names.empty());
  EXPECT_EQ(transformer.begin(), transformer.end());
  EXPECT_EQ(transformer.cbegin(), transformer.cend());
}

TEST(JSONSchema_transformer_iterator, single_rule_iteration) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<ExampleRule1>();

  std::set<std::string> rule_names;
  for (const auto &entry : transformer) {
    rule_names.insert(entry.first);
  }

  EXPECT_EQ(rule_names.size(), 1);
  EXPECT_TRUE(rule_names.contains("example_rule_1"));
}

TEST(JSONSchema_transformer_iterator, multiple_rules_iteration) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<ExampleRule1>();
  transformer.add<ExampleRule2>();
  transformer.add<ExampleRule3>();

  std::set<std::string> rule_names;
  for (const auto &entry : transformer) {
    rule_names.insert(entry.first);
  }

  EXPECT_EQ(rule_names.size(), 3);
  EXPECT_TRUE(rule_names.contains("example_rule_1"));
  EXPECT_TRUE(rule_names.contains("example_rule_2"));
  EXPECT_TRUE(rule_names.contains("example_rule_3"));
}

TEST(JSONSchema_transformer_iterator, const_transformer_iteration) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<ExampleRule1>();
  transformer.add<ExampleRule2>();

  const auto &const_transformer = transformer;

  std::set<std::string> rule_names;
  for (const auto &entry : const_transformer) {
    rule_names.insert(std::string{entry.first});
  }

  EXPECT_EQ(rule_names.size(), 2);
  EXPECT_TRUE(rule_names.contains("example_rule_1"));
  EXPECT_TRUE(rule_names.contains("example_rule_2"));
}

TEST(JSONSchema_transformer_iterator, explicit_iterator_usage) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<ExampleRule1>();
  transformer.add<ExampleRule2>();

  std::set<std::string> rule_names;
  for (auto it = transformer.begin(); it != transformer.end(); ++it) {
    rule_names.insert(it->first);
  }

  EXPECT_EQ(rule_names.size(), 2);
  EXPECT_TRUE(rule_names.contains("example_rule_1"));
  EXPECT_TRUE(rule_names.contains("example_rule_2"));
}

TEST(JSONSchema_transformer_iterator, cbegin_cend_usage) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<ExampleRule3>();

  std::set<std::string> rule_names;
  for (auto it = transformer.cbegin(); it != transformer.cend(); ++it) {
    rule_names.insert(it->first);
  }

  EXPECT_EQ(rule_names.size(), 1);
  EXPECT_TRUE(rule_names.contains("example_rule_3"));
}

TEST(JSONSchema_transformer_iterator, rule_access_read_only) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<ExampleRule1>();

  for (const auto &entry : transformer) {
    EXPECT_EQ(entry.first, "example_rule_1");
    EXPECT_NE(entry.second.get(), nullptr);
    EXPECT_EQ(entry.second->name(), "example_rule_1");
    EXPECT_EQ(entry.second->message(), "Keyword foo is not permitted");
  }
}
