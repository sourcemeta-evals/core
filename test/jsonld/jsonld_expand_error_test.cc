#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>

TEST(JSONLD_expand_error, invalid_id_value_number) {
  const auto input{sourcemeta::core::parse_json(R"({ "@id": 42 })")};
  try {
    static_cast<void>(sourcemeta::core::jsonld_expand(input));
    FAIL() << "Expected JSONLDError";
  } catch (const sourcemeta::core::JSONLDError &error) {
    EXPECT_STREQ(error.what(), "Invalid @id value");
  }
}
