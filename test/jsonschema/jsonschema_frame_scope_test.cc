#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

// A meta-schema a schema carries speaks for everything below whatever holds
// it, so a chain of them may have its links held by different schemas: one
// carried where the reading sits, the next by a schema enclosing it
TEST(a_dialect_whose_chain_a_resource_above_it_holds) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://example.com/schema",
    "$defs": {
      "outer": {
        "$id": "https://example.com/outer",
        "$schema": "https://example.com/mid",
        "$defs": {
          "https://example.com/mid": {
            "$id": "https://example.com/mid",
            "$schema": "https://json-schema.org/draft/2020-12/schema",
            "$vocabulary": {
              "https://json-schema.org/draft/2020-12/vocab/core": true,
              "https://json-schema.org/draft/2020-12/vocab/applicator": true
            }
          },
          "inner": {
            "$id": "https://example.com/inner",
            "$schema": "https://example.com/deep",
            "$defs": {
              "https://example.com/deep": {
                "$id": "https://example.com/deep",
                "$schema": "https://example.com/mid",
                "$vocabulary": {
                  "https://json-schema.org/draft/2020-12/vocab/core": true,
                  "https://json-schema.org/draft/2020-12/vocab/applicator": true
                }
              }
            }
          }
        }
      }
    }
  })JSON");

  const sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References, document,
      sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver};

  const auto inner{frame.traverse("https://example.com/inner")};
  EXPECT_TRUE(inner.has_value());
  const auto &vocabularies{
      frame.vocabularies(inner->get(), sourcemeta::core::schema_resolver)};
  EXPECT_EQ(vocabularies.size(), 2);
  EXPECT_TRUE(vocabularies.contains(
      sourcemeta::core::SchemaVocabularies::Known::JSON_SCHEMA_2020_12_CORE));
  EXPECT_TRUE(vocabularies.contains(sourcemeta::core::SchemaVocabularies::
                                        Known::JSON_SCHEMA_2020_12_APPLICATOR));
}
