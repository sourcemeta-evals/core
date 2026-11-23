#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/test.h>

#include "../../../src/spec/jsonld/jsonld_algorithms.h"

#include <optional> // std::optional, std::nullopt

// The Value Compaction defensive paths cannot be produced through the public
// entry points, as expansion refuses the shapes that reach them, so they are
// exercised directly to complete the module's coverage

TEST(value_compaction_keeps_a_non_string_type) {
  const sourcemeta::core::ActiveContext active_context;
  const auto inverse_context{
      sourcemeta::core::create_inverse_context(active_context)};
  const auto value = sourcemeta::core::parse_json(R"({
    "@type": 5, "@value": 1
  })");

  const auto result{sourcemeta::core::compact_value(
      active_context, inverse_context, std::nullopt, value)};
  EXPECT_TRUE(result.is_object());
  EXPECT_TRUE(result.defines("@type"));
}

TEST(value_compaction_keeps_a_non_string_identifier) {
  const sourcemeta::core::ActiveContext active_context;
  const auto inverse_context{
      sourcemeta::core::create_inverse_context(active_context)};
  const auto value = sourcemeta::core::parse_json(R"({
    "@id": 7
  })");

  const auto result{sourcemeta::core::compact_value(
      active_context, inverse_context, std::nullopt, value)};
  EXPECT_TRUE(result.is_object());
}
