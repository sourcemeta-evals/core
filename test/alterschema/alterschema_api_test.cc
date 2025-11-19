#include <gtest/gtest.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/core/jsonschema_transform.h>

class MockRule : public sourcemeta::core::SchemaTransformRule {
public:
  MockRule(std::string name)
      : sourcemeta::core::SchemaTransformRule(std::move(name), "Mock rule") {}

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> sourcemeta::core::SchemaTransformRule::Result override {
    return false;
  }
};

TEST(SchemaTransformer, Introspection) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<MockRule>("rule1");
  transformer.add<MockRule>("rule2");

  const auto &rules = transformer.rules();
  EXPECT_EQ(rules.size(), 2);
  EXPECT_TRUE(rules.contains("rule1"));
  EXPECT_TRUE(rules.contains("rule2"));
  EXPECT_EQ(rules.at("rule1")->name(), "rule1");
  EXPECT_EQ(rules.at("rule2")->name(), "rule2");
}
