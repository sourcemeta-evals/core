#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonpointer.h>

TEST(JSONLD_expand, context_term_expands_to_iri) {
  const auto input{sourcemeta::core::parse_json(R"({
    "@context": { "name": "https://schema.org/name" },
    "name": "Sourcemeta"
  })")};
  const auto expected{sourcemeta::core::parse_json(R"([
    { "https://schema.org/name": [ { "@value": "Sourcemeta" } ] }
  ])")};
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(JSONLD_expand_error, invalid_id_value_throws) {
  const auto input{sourcemeta::core::parse_json(R"({
    "@id": 42
  })")};
  try {
    static_cast<void>(sourcemeta::core::jsonld_expand(input));
    FAIL() << "Expected JSONLDError to be thrown";
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_STREQ(error.what(), "Invalid @id value");
  }
}
