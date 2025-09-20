#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <string>
#include <vector>

#include "jsonschema_transform_rules.h"

TEST(JSONSchema_transformer, iterate_rules_readonly) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule2>();
  bundle.add<ExampleRule1>();

  std::vector<std::string> names;
  for (const auto &entry : bundle) {
    names.push_back(entry.first);
    EXPECT_NE(entry.second.get(), nullptr);
  }

  ASSERT_EQ(names.size(), 2);
  EXPECT_EQ(names[0], "example_rule_1");
  EXPECT_EQ(names[1], "example_rule_2");
}

TEST(JSONSchema_transformer, iterate_rules_const_bundle) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule2>();

  const sourcemeta::core::SchemaTransformer &cbundle = bundle;

  std::vector<std::string> names;
  for (auto it = cbundle.cbegin(); it != cbundle.cend(); ++it) {
    names.push_back(it->first);
    EXPECT_NE(it->second.get(), nullptr);
  }

  ASSERT_EQ(names.size(), 2);
  EXPECT_EQ(names[0], "example_rule_1");
  EXPECT_EQ(names[1], "example_rule_2");
}

TEST(JSONSchema_transformer, iterate_rules_empty_bundle) {
  sourcemeta::core::SchemaTransformer bundle;

  std::vector<std::string> names;
  for (const auto &entry : bundle) {
    names.push_back(entry.first);
  }

  EXPECT_TRUE(names.empty());
}

TEST(JSONSchema_transformer, iterate_rules_single_rule) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule3>();

  std::vector<std::string> names;
  std::vector<std::string> messages;
  for (const auto &entry : bundle) {
    names.push_back(entry.first);
    messages.push_back(entry.second->message());
  }

  ASSERT_EQ(names.size(), 1);
  EXPECT_EQ(names[0], "example_rule_3");
  EXPECT_EQ(messages[0], "Example rule 3");
}
