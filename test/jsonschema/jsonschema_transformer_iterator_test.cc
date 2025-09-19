#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <algorithm>
#include <string>
#include <vector>

#include "jsonschema_transform_rules.h"

TEST(JSONSchema_transformer, iterate_rules_const_only) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule2>();

  // Only allow iteration on const reference (read-only introspection)
  const auto &cbundle = bundle;

  std::vector<std::string> names;
  for (const auto &entry : cbundle) {
    names.push_back(entry.first);
  }

  // Rules are stored in std::map keyed by rule name, so the result is sorted
  ASSERT_EQ(names.size(), 2U);
  EXPECT_EQ(names[0], "example_rule_1");
  EXPECT_EQ(names[1], "example_rule_2");
}

TEST(JSONSchema_transformer, iterate_empty_bundle) {
  const sourcemeta::core::SchemaTransformer bundle;

  std::vector<std::string> names;
  for (const auto &entry : bundle) {
    names.push_back(entry.first);
  }

  EXPECT_TRUE(names.empty());
}

TEST(JSONSchema_transformer, iterate_single_rule) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule3>();

  const auto &cbundle = bundle;

  std::vector<std::string> names;
  for (const auto &entry : cbundle) {
    names.push_back(entry.first);
  }

  ASSERT_EQ(names.size(), 1U);
  EXPECT_EQ(names[0], "example_rule_3");
}
