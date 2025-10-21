#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <set>
#include <string>

// Test-only rule classes
class IntrospectionTestRule1 final
    : public sourcemeta::core::SchemaTransformRule {
public:
  IntrospectionTestRule1()
      : sourcemeta::core::SchemaTransformRule("introspection_test_rule_1",
                                              "Test rule 1") {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    return schema.defines("test1");
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.erase("test1");
  }
};

class IntrospectionTestRule2 final
    : public sourcemeta::core::SchemaTransformRule {
public:
  IntrospectionTestRule2()
      : sourcemeta::core::SchemaTransformRule("introspection_test_rule_2",
                                              "Test rule 2") {};

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &schema,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    return schema.defines("test2");
  }

  auto transform(sourcemeta::core::JSON &schema) const -> void override {
    schema.erase("test2");
  }
};

TEST(JSONSchema_transformer_introspection, empty_transformer) {
  sourcemeta::core::SchemaTransformer transformer;

  EXPECT_EQ(transformer.size(), 0);

  std::size_t visit_count = 0;
  transformer.visit([&visit_count](const auto &) { visit_count++; });

  EXPECT_EQ(visit_count, 0);
}

TEST(JSONSchema_transformer_introspection, single_rule) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<IntrospectionTestRule1>();

  EXPECT_EQ(transformer.size(), 1);

  std::size_t visit_count = 0;
  std::set<std::string> rule_names;
  transformer.visit([&visit_count, &rule_names](const auto &rule) {
    visit_count++;
    rule_names.insert(rule.name());
  });

  EXPECT_EQ(visit_count, 1);
  EXPECT_EQ(rule_names.size(), 1);
  EXPECT_TRUE(rule_names.contains("introspection_test_rule_1"));
}

TEST(JSONSchema_transformer_introspection, multiple_rules) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<IntrospectionTestRule1>();
  transformer.add<IntrospectionTestRule2>();

  EXPECT_EQ(transformer.size(), 2);

  std::size_t visit_count = 0;
  std::set<std::string> rule_names;
  std::set<std::string> rule_messages;
  transformer.visit(
      [&visit_count, &rule_names, &rule_messages](const auto &rule) {
        visit_count++;
        rule_names.insert(rule.name());
        rule_messages.insert(rule.message());
      });

  EXPECT_EQ(visit_count, 2);
  EXPECT_EQ(rule_names.size(), 2);
  EXPECT_TRUE(rule_names.contains("introspection_test_rule_1"));
  EXPECT_TRUE(rule_names.contains("introspection_test_rule_2"));
  EXPECT_EQ(rule_messages.size(), 2);
  EXPECT_TRUE(rule_messages.contains("Test rule 1"));
  EXPECT_TRUE(rule_messages.contains("Test rule 2"));
}

TEST(JSONSchema_transformer_introspection, after_remove) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<IntrospectionTestRule1>();
  transformer.add<IntrospectionTestRule2>();

  EXPECT_EQ(transformer.size(), 2);

  transformer.remove("introspection_test_rule_1");

  EXPECT_EQ(transformer.size(), 1);

  std::size_t visit_count = 0;
  std::set<std::string> rule_names;
  transformer.visit([&visit_count, &rule_names](const auto &rule) {
    visit_count++;
    rule_names.insert(rule.name());
  });

  EXPECT_EQ(visit_count, 1);
  EXPECT_EQ(rule_names.size(), 1);
  EXPECT_FALSE(rule_names.contains("introspection_test_rule_1"));
  EXPECT_TRUE(rule_names.contains("introspection_test_rule_2"));
}
