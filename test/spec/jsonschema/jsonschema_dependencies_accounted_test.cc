#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

// The fixture suite answers for a fixed list of URIs, so what it cannot
// express is a caller whose answer changes while the walk runs, which is what
// a caller stitching several documents into one span does. Only then does a
// reference that nothing answered for reach the end of the walk and settle
// against what the caller has since come to hold

static auto sibling_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/sibling") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://www.sourcemeta.com/sibling",
      "$defs": {
        "inner": {
          "$id": "https://www.sourcemeta.com/sibling-inner",
          "type": "string"
        }
      }
    })JSON");
  }

  return sourcemeta::core::schema_resolver(identifier);
}

TEST(a_reference_the_caller_comes_to_account_for_settles_at_the_end) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "https://www.sourcemeta.com/elsewhere" },
    "contains": { "$ref": "https://www.sourcemeta.com/sibling" }
  })JSON");

  // Reading the sibling is what leaves the caller holding the document that
  // the other reference names, which it only knows once the walk says so
  std::unordered_set<std::string> held;
  sourcemeta::core::SchemaDependenciesOptions options;
  options.accounted = [&held](const std::string_view identifier) -> bool {
    return held.contains(std::string{identifier});
  };
  options.accounted_callback =
      [&held](const std::string_view identifier) -> void {
    held.emplace(identifier);
    if (identifier == "https://www.sourcemeta.com/sibling-inner") {
      held.emplace("https://www.sourcemeta.com/elsewhere");
    }
  };

  [[maybe_unused]] const auto locations{sourcemeta::core::schema_dependencies(
      document, sourcemeta::core::schema_walker, sibling_resolver,
      [](const auto &) -> void {}, options)};

  EXPECT_TRUE(held.contains("https://www.sourcemeta.com/elsewhere"));
}

TEST(a_reference_the_caller_never_accounts_for_ends_analysis) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "https://www.sourcemeta.com/elsewhere" },
    "contains": { "$ref": "https://www.sourcemeta.com/sibling" }
  })JSON");

  std::unordered_set<std::string> held;
  sourcemeta::core::SchemaDependenciesOptions options;
  options.accounted = [&held](const std::string_view identifier) -> bool {
    return held.contains(std::string{identifier});
  };
  options.accounted_callback =
      [&held](const std::string_view identifier) -> void {
    held.emplace(identifier);
  };

  try {
    [[maybe_unused]] const auto locations{sourcemeta::core::schema_dependencies(
        document, sourcemeta::core::schema_walker, sibling_resolver,
        [](const auto &) -> void {}, options)};
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_EQ(error.identifier(), "https://www.sourcemeta.com/elsewhere");
  }
}

static auto chained_leaf_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/leaf") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://www.sourcemeta.com/leaf",
      "items": { "$ref": "https://www.sourcemeta.com/inner" }
    })JSON");
  }

  if (identifier == "https://www.sourcemeta.com/inner") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://www.sourcemeta.com/inner",
      "type": "string"
    })JSON");
  }

  return sourcemeta::core::schema_resolver(identifier);
}

TEST(a_document_already_reported_is_read_whatever_the_predicate_says) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "items": { "$ref": "https://www.sourcemeta.com/leaf" }
  })JSON");

  // The caller comes to account for the leaf the moment it is reported, which
  // must not keep the walk from reading it: the read follows from the report,
  // so the reference the leaf holds is still examined
  std::unordered_set<std::string> held;
  std::vector<std::string> reported;
  sourcemeta::core::SchemaDependenciesOptions options;
  options.accounted = [&held](const std::string_view identifier) -> bool {
    return held.contains(std::string{identifier});
  };

  [[maybe_unused]] const auto locations{sourcemeta::core::schema_dependencies(
      document, sourcemeta::core::schema_walker, chained_leaf_resolver,
      [&held, &reported](
          const sourcemeta::core::SchemaDependency &dependency) -> void {
        reported.emplace_back(dependency.destination);
        if (dependency.destination == "https://www.sourcemeta.com/leaf") {
          held.emplace("https://www.sourcemeta.com/leaf");
        }
      },
      options)};

  EXPECT_EQ(reported.size(), 2);
  EXPECT_EQ(reported.at(0), "https://www.sourcemeta.com/leaf");
  EXPECT_EQ(reported.at(1), "https://www.sourcemeta.com/inner");
}
