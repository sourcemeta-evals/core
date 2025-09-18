#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <string>
#include <type_traits>
#include <vector>

#include "jsonschema_transform_rules.h"

TEST(JSONSchema_transformer, iterate_rules_read_only) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();
  bundle.add<ExampleRule2>();

  // Basic introspection helpers
  EXPECT_FALSE(bundle.empty());
  EXPECT_GE(bundle.size(), static_cast<std::size_t>(2));

  // Const iteration via cbegin/cend
  std::vector<std::string> keys;
  std::vector<std::string> names;
  for (auto it = bundle.cbegin(); it != bundle.cend(); ++it) {
    keys.emplace_back(it->first);
    names.emplace_back(it->second->name());
  }
  EXPECT_EQ(keys.size(), bundle.size());
  EXPECT_EQ(keys, names);

  // Range-based for on const reference
  const auto &cbundle = bundle;
  std::vector<std::string> msgs;
  for (const auto &[name, rule] : cbundle) {
    EXPECT_EQ(name, rule->name());
    (void)rule->message();
    msgs.emplace_back(rule->message());
  }
  EXPECT_EQ(msgs.size(), cbundle.size());

  // Verify iterator type is the public const_iterator
  using IterT = decltype(cbundle.begin());
  static_assert(
      std::is_same_v<IterT,
                     sourcemeta::core::SchemaTransformer::const_iterator>,
      "begin() should return const_iterator");
}

TEST(JSONSchema_transformer, empty_transformer_iteration) {
  sourcemeta::core::SchemaTransformer bundle;

  EXPECT_TRUE(bundle.empty());
  EXPECT_EQ(bundle.size(), static_cast<std::size_t>(0));

  // Empty iteration
  EXPECT_EQ(bundle.begin(), bundle.end());
  EXPECT_EQ(bundle.cbegin(), bundle.cend());

  // Range-based for should not execute
  for (const auto &[name, rule] : bundle) {
    FAIL() << "Should not iterate over empty transformer";
  }
}

TEST(JSONSchema_transformer, single_rule_iteration) {
  sourcemeta::core::SchemaTransformer bundle;
  bundle.add<ExampleRule1>();

  EXPECT_FALSE(bundle.empty());
  EXPECT_EQ(bundle.size(), static_cast<std::size_t>(1));

  // Single iteration
  auto it = bundle.begin();
  EXPECT_NE(it, bundle.end());
  EXPECT_EQ(it->first, it->second->name());
  ++it;
  EXPECT_EQ(it, bundle.end());
}
