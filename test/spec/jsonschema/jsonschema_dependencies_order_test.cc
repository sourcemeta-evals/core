#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include <stdexcept>
#include <string>
#include <string_view>

// The fixture suite records what was fetched, what was reported, and what was
// accounted for as three separate lists, so what it cannot express is how the
// three interleave: a walk that fetched every sibling before reporting any,
// or accounted for a document before reporting it, would satisfy every list
// while observing events in the wrong order. Only one shared log can pin that

static auto chain_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/alpha") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://www.sourcemeta.com/alpha",
      "items": { "$ref": "https://www.sourcemeta.com/gamma" }
    })JSON");
  }

  if (identifier == "https://www.sourcemeta.com/beta") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://www.sourcemeta.com/beta",
      "type": "string"
    })JSON");
  }

  if (identifier == "https://www.sourcemeta.com/gamma") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "$id": "https://www.sourcemeta.com/gamma",
      "type": "integer"
    })JSON");
  }

  return sourcemeta::core::schema_resolver(identifier);
}

TEST(dependencies_fetches_reports_and_accounting_interleave_in_event_order) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.sourcemeta.com/entry",
    "$defs": {
      "one": { "$ref": "https://www.sourcemeta.com/alpha" },
      "two": { "$ref": "https://www.sourcemeta.com/beta" }
    }
  })JSON");

  std::string events;
  const sourcemeta::core::SchemaResolver resolver{
      [&events](const std::string_view identifier)
          -> sourcemeta::core::SchemaResolverResult {
        if (!sourcemeta::core::schema_is_official(identifier)) {
          events += "fetch ";
          events += identifier;
          events += '\n';
        }

        return chain_resolver(identifier);
      }};

  sourcemeta::core::SchemaDependenciesOptions options;
  options.accounted_callback =
      [&events](const std::string_view identifier) -> void {
    events += "account ";
    events += identifier;
    events += '\n';
  };

  [[maybe_unused]] const auto locations{sourcemeta::core::schema_dependencies(
      document, sourcemeta::core::schema_walker, resolver,
      [&events](const sourcemeta::core::SchemaDependency &dependency) -> void {
        events += "report ";
        events += dependency.destination;
        events += '\n';
      },
      options)};

  EXPECT_EQ(events, "account https://www.sourcemeta.com/entry\n"
                    "fetch https://www.sourcemeta.com/alpha\n"
                    "report https://www.sourcemeta.com/alpha\n"
                    "account https://www.sourcemeta.com/alpha\n"
                    "fetch https://www.sourcemeta.com/beta\n"
                    "report https://www.sourcemeta.com/beta\n"
                    "account https://www.sourcemeta.com/beta\n"
                    "fetch https://www.sourcemeta.com/gamma\n"
                    "report https://www.sourcemeta.com/gamma\n"
                    "account https://www.sourcemeta.com/gamma\n");
}

TEST(an_exception_the_callback_throws_travels_unchanged_and_stops_the_walk) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.sourcemeta.com/entry",
    "$defs": {
      "one": { "$ref": "https://www.sourcemeta.com/alpha" },
      "two": { "$ref": "https://www.sourcemeta.com/beta" }
    }
  })JSON");

  std::string events;
  const sourcemeta::core::SchemaResolver resolver{
      [&events](const std::string_view identifier)
          -> sourcemeta::core::SchemaResolverResult {
        if (!sourcemeta::core::schema_is_official(identifier)) {
          events += "fetch ";
          events += identifier;
          events += '\n';
        }

        return chain_resolver(identifier);
      }};

  sourcemeta::core::SchemaDependenciesOptions options;
  options.accounted_callback =
      [&events](const std::string_view identifier) -> void {
    events += "account ";
    events += identifier;
    events += '\n';
  };

  try {
    [[maybe_unused]] const auto locations{sourcemeta::core::schema_dependencies(
        document, sourcemeta::core::schema_walker, resolver,
        [](const sourcemeta::core::SchemaDependency &) -> void {
          throw std::runtime_error{"the caller wants out"};
        },
        options)};
    FAIL();
  } catch (const std::runtime_error &error) {
    EXPECT_STREQ(error.what(), "the caller wants out");
  }

  // The first report threw, so its document was never accounted for and its
  // sibling was never fetched
  EXPECT_EQ(events, "account https://www.sourcemeta.com/entry\n"
                    "fetch https://www.sourcemeta.com/alpha\n");
}
