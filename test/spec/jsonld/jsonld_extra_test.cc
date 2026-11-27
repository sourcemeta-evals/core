#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/test.h>

#include <string>

static auto parse_document(const char *text) -> sourcemeta::core::JSON {
  return sourcemeta::core::parse_json(text);
}

TEST(extra_expansion_of_a_mapped_term) {
  const auto input =
      parse_document(R"({"@context":{"p":"http://example.com/p"},"p":"x"})");
  const auto expected =
      parse_document(R"([{"http://example.com/p":[{"@value":"x"}]}])");
  EXPECT_EQ(sourcemeta::core::jsonld_expand(input), expected);
}

TEST(extra_expansion_rejects_a_cyclic_term) {
  const auto input =
      parse_document(R"({"@context":{"term":{"@id":"term:term"}}})");
  try {
    [[maybe_unused]] const auto result{sourcemeta::core::jsonld_expand(input)};
  } catch (const sourcemeta::core::JSONLDError &error) {
    const std::string message{error.what()};
    EXPECT_EQ(message, "Cyclic IRI mapping");
  }
}
