#include <gtest/gtest.h>
#include <sourcemeta/core/jsonschema_frame.h>
#include <sourcemeta/core/jsonschema_transform.h>
#include <string>

namespace {

class TestRule : public sourcemeta::core::SchemaTransformRule {
public:
  TestRule(std::string name)
      : SchemaTransformRule(std::move(name), "Test rule") {}

  [[nodiscard]] auto condition(const sourcemeta::core::JSON &,
                               const sourcemeta::core::JSON &,
                               const sourcemeta::core::Vocabularies &,
                               const sourcemeta::core::SchemaFrame &,
                               const sourcemeta::core::SchemaFrame::Location &,
                               const sourcemeta::core::SchemaWalker &,
                               const sourcemeta::core::SchemaResolver &) const
      -> Result override {
    return false;
  }
};

} // namespace

TEST(SchemaTransformer, Iteration) {
  sourcemeta::core::SchemaTransformer transformer;
  transformer.add<TestRule>("rule1");
  transformer.add<TestRule>("rule2");

  int count = 0;
  bool found_rule1 = false;
  bool found_rule2 = false;

  for (auto it = transformer.begin(); it != transformer.end(); ++it) {
    count++;
    if (it->first == "rule1")
      found_rule1 = true;
    if (it->first == "rule2")
      found_rule2 = true;

    // Check that we can access the rule
    EXPECT_EQ(it->second->name(), it->first);
    EXPECT_EQ(it->second->message(), "Test rule");
  }

  EXPECT_EQ(count, 2);
  EXPECT_TRUE(found_rule1);
  EXPECT_TRUE(found_rule2);
}
