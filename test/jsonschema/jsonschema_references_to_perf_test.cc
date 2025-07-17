#include <gtest/gtest.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include "jsonschema_test_utils.h"

TEST(JSONSchema_references_to_perf, many_references_to_same_location) {
  const sourcemeta::core::JSON document = sourcemeta::core::parse_json(R"JSON({
    "$id": "https://example.com",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "properties": {
      "target": {
        "$anchor": "shared"
      }
    },
    "$defs": {
      "ref1": { "$ref": "#shared" },
      "ref2": { "$ref": "#shared" },
      "ref3": { "$ref": "#shared" },
      "ref4": { "$ref": "#shared" },
      "ref5": { "$ref": "#shared" },
      "ref6": { "$ref": "#shared" },
      "ref7": { "$ref": "#shared" },
      "ref8": { "$ref": "#shared" },
      "ref9": { "$ref": "#shared" },
      "ref10": { "$ref": "#shared" }
    }
  })JSON");

  sourcemeta::core::SchemaFrame frame{
      sourcemeta::core::SchemaFrame::Mode::References};
  frame.analyse(document, sourcemeta::core::schema_official_walker,
                sourcemeta::core::schema_official_resolver);

  const auto target_pointer = sourcemeta::core::Pointer{"properties", "target"};

  // First call should build the cache
  const auto refs1{frame.references_to(target_pointer)};
  EXPECT_EQ(refs1.size(), 10U);

  // Verify all references are static and point to the expected destination
  for (const auto &ref : refs1) {
    EXPECT_EQ(ref.get().first.first,
              sourcemeta::core::SchemaReferenceType::Static);
    EXPECT_EQ(ref.get().second.destination, "https://example.com#shared");
  }

  // Subsequent calls should use the cache and return identical results
  const auto refs2{frame.references_to(target_pointer)};
  EXPECT_EQ(refs2.size(), 10U);
  EXPECT_EQ(refs1.size(), refs2.size());

  for (int i = 0; i < 5; i++) {
    const auto refs_repeat{frame.references_to(target_pointer)};
    EXPECT_EQ(refs_repeat.size(), 10U);
  }

  // Pointer with no references
  const sourcemeta::core::Pointer nonexistent_pointer{"nonexistent"};
  const auto no_refs{frame.references_to(nonexistent_pointer)};
  EXPECT_TRUE(no_refs.empty());
}
