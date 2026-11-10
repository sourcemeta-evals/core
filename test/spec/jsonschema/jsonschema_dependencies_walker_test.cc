#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include <string>
#include <string_view>
#include <vector>

// The fixture suite walks every fixture with the standard walker, so what it
// cannot express is a caller whose own walker declares further keywords to
// hold schemas. References beneath such a keyword are reached only where the
// walker argument actually drives the traversal

static auto x_child_walker(const std::string_view keyword,
                           const sourcemeta::core::SchemaVocabularies
                               &vocabularies)
    -> const sourcemeta::core::SchemaWalkerResult & {
  if (keyword == "x-child") {
    static const sourcemeta::core::SchemaWalkerResult result{
        sourcemeta::core::SchemaKeywordType::ApplicatorValueTraverseAnyItem,
        std::nullopt,
        {},
        {},
        {}};
    return result;
  }

  return sourcemeta::core::schema_walker(keyword, vocabularies);
}

static auto x_child_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/remote") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://www.sourcemeta.com/remote",
      "x-child": { "$ref": "https://www.sourcemeta.com/leaf" }
    })JSON");
  }

  if (identifier == "https://www.sourcemeta.com/leaf") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://www.sourcemeta.com/leaf",
      "type": "string"
    })JSON");
  }

  return sourcemeta::core::schema_resolver(identifier);
}

TEST(a_caller_supplied_walker_reaches_references_of_its_own) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.sourcemeta.com/entry",
    "x-child": { "$ref": "https://www.sourcemeta.com/remote" }
  })JSON");

  std::vector<std::string> reported;
  [[maybe_unused]] const auto locations{sourcemeta::core::schema_dependencies(
      document, x_child_walker, x_child_resolver,
      [&reported](const sourcemeta::core::SchemaDependency &dependency)
          -> void { reported.emplace_back(dependency.destination); })};

  // The entry reaches the remote through the caller's keyword, and the
  // remote reaches the leaf the same way, so honoring the walker matters
  // for fetched documents too
  EXPECT_EQ(reported.size(), 2);
  EXPECT_EQ(reported.at(0), "https://www.sourcemeta.com/remote");
  EXPECT_EQ(reported.at(1), "https://www.sourcemeta.com/leaf");
}

TEST(the_standard_walker_leaves_unknown_keywords_alone) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.sourcemeta.com/entry",
    "x-child": { "$ref": "https://www.sourcemeta.com/remote" }
  })JSON");

  std::vector<std::string> reported;
  [[maybe_unused]] const auto locations{sourcemeta::core::schema_dependencies(
      document, sourcemeta::core::schema_walker, x_child_resolver,
      [&reported](const sourcemeta::core::SchemaDependency &dependency)
          -> void { reported.emplace_back(dependency.destination); })};

  EXPECT_EQ(reported.size(), 0);
}
