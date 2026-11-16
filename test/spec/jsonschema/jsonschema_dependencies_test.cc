#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

#include <string>      // std::string
#include <string_view> // std::string_view
#include <vector>      // std::vector

static auto test_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.example.com/remote") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://www.example.com/remote",
      "type": "string"
    })JSON");
  }

  return sourcemeta::core::schema_resolver(identifier);
}

TEST(dependencies_reports_a_remote_reference) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.example.com/entry",
    "items": { "$ref": "https://www.example.com/remote" }
  })JSON")};

  std::vector<std::string> destinations;
  const auto count{sourcemeta::core::schema_dependencies(
      document, sourcemeta::core::schema_walker, test_resolver,
      [&destinations](const auto &dependency) {
        destinations.emplace_back(dependency.destination);
      })};

  EXPECT_EQ(count, 1);
  EXPECT_EQ(destinations.size(), 1);
  EXPECT_EQ(destinations.front(), "https://www.example.com/remote");
}

TEST(dependencies_reports_nothing_without_references) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.example.com/entry",
    "type": "string"
  })JSON")};

  std::vector<std::string> destinations;
  const auto count{sourcemeta::core::schema_dependencies(
      document, sourcemeta::core::schema_walker, test_resolver,
      [&destinations](const auto &dependency) {
        destinations.emplace_back(dependency.destination);
      })};

  EXPECT_EQ(count, 0);
  EXPECT_TRUE(destinations.empty());
}

TEST(dependencies_reports_a_transitive_reference_once) {
  const auto document{sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.example.com/entry",
    "properties": {
      "first": { "$ref": "https://www.example.com/remote" },
      "second": { "$ref": "https://www.example.com/remote" }
    }
  })JSON")};

  std::vector<std::string> destinations;
  const auto count{sourcemeta::core::schema_dependencies(
      document, sourcemeta::core::schema_walker, test_resolver,
      [&destinations](const auto &dependency) {
        destinations.emplace_back(dependency.destination);
      })};

  EXPECT_EQ(count, 1);
  EXPECT_EQ(destinations.size(), 1);
}
