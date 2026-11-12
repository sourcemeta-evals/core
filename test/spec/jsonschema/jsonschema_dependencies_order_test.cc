#include <sourcemeta/core/test.h>

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/jsonschema.h>

#include <sourcemeta/core/uri.h>

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

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

// A remote written against a dialect of its own, which reading the remote has
// to ask the resolver about
static auto custom_dialect_remote_resolver(std::string_view identifier)
    -> sourcemeta::core::SchemaResolverResult {
  if (identifier == "https://www.sourcemeta.com/custom-remote") {
    return sourcemeta::core::parse_json(R"JSON({
      "$schema": "https://www.sourcemeta.com/custom-dialect",
      "$id": "https://www.sourcemeta.com/custom-remote",
      "type": "string"
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

// Deciding whether a fetched document can be read at all asks the resolver
// for its dialect, inside the handlers that translate the library's own
// lookup failures, so an exception of the caller's thrown right there is
// where translation would lose it. The contract is that it travels out
// unchanged, with nothing observed past it
TEST(an_exception_the_resolver_throws_in_a_dialect_lookup_is_unchanged) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.sourcemeta.com/entry",
    "items": { "$ref": "https://www.sourcemeta.com/custom-remote" }
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

        if (identifier == "https://www.sourcemeta.com/custom-dialect") {
          throw sourcemeta::core::SchemaResolutionError{
              "the caller could not resolve this meta-schema", identifier};
        }

        return custom_dialect_remote_resolver(identifier);
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
        [&events](
            const sourcemeta::core::SchemaDependency &dependency) -> void {
          events += "report ";
          events += dependency.destination;
          events += '\n';
        },
        options)};
    FAIL();
  } catch (const sourcemeta::core::SchemaResolutionError &error) {
    EXPECT_STREQ(error.what(), "the caller could not resolve this meta-schema");
    EXPECT_EQ(error.identifier(), "https://www.sourcemeta.com/custom-dialect");
  }

  // The dialect lookup threw, so the remote was never reported or accounted
  EXPECT_EQ(events, "account https://www.sourcemeta.com/entry\n"
                    "fetch https://www.sourcemeta.com/custom-remote\n"
                    "fetch https://www.sourcemeta.com/custom-dialect\n");
}

TEST(a_dialect_error_the_resolver_throws_in_a_dialect_lookup_is_unchanged) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.sourcemeta.com/entry",
    "items": { "$ref": "https://www.sourcemeta.com/custom-remote" }
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

        if (identifier == "https://www.sourcemeta.com/custom-dialect") {
          throw sourcemeta::core::SchemaUnknownBaseDialectError{};
        }

        return custom_dialect_remote_resolver(identifier);
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
        [&events](
            const sourcemeta::core::SchemaDependency &dependency) -> void {
          events += "report ";
          events += dependency.destination;
          events += '\n';
        },
        options)};
    FAIL();
  } catch (const sourcemeta::core::SchemaUnknownBaseDialectError &error) {
    EXPECT_STREQ(error.what(),
                 "Could not determine the base dialect of the schema");
  }

  EXPECT_EQ(events, "account https://www.sourcemeta.com/entry\n"
                    "fetch https://www.sourcemeta.com/custom-remote\n"
                    "fetch https://www.sourcemeta.com/custom-dialect\n");
}

// The control for the two cases above: the same walk, with the dialect lookup
// answered with nothing rather than thrown at, proving the unchanged
// propagation above comes from the caller's throw alone. A lookup that fails
// on its own is the library's failure and reads as the documented reference
// error
TEST(a_dialect_nothing_answers_for_reads_as_no_schema) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.sourcemeta.com/entry",
    "items": { "$ref": "https://www.sourcemeta.com/custom-remote" }
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

        if (identifier == "https://www.sourcemeta.com/custom-dialect") {
          return std::nullopt;
        }

        return custom_dialect_remote_resolver(identifier);
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
        [&events](
            const sourcemeta::core::SchemaDependency &dependency) -> void {
          events += "report ";
          events += dependency.destination;
          events += '\n';
        },
        options)};
    FAIL();
  } catch (const sourcemeta::core::SchemaReferenceError &error) {
    EXPECT_STREQ(error.what(), "The JSON document is not a valid JSON Schema");
    EXPECT_EQ(error.identifier(), "https://www.sourcemeta.com/custom-remote");
  }

  EXPECT_EQ(events, "account https://www.sourcemeta.com/entry\n"
                    "fetch https://www.sourcemeta.com/custom-remote\n"
                    "fetch https://www.sourcemeta.com/custom-dialect\n");
}

// A remote that overrides an official schema may declare a dialect of its
// own, and deciding whether that remote can be read asks the resolver for
// that dialect, so a parser error of the caller's thrown right there is
// where translation would lose it. The contract is that it travels out
// unchanged, with nothing observed past it
TEST(a_parser_error_the_resolver_throws_in_a_dialect_lookup_is_unchanged) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.sourcemeta.com/entry",
    "items": { "$ref": "http://json-schema.org/draft-06/schema" }
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

        if (identifier == "https://www.sourcemeta.com/custom-dialect") {
          throw sourcemeta::core::URIParseError{7};
        }

        if (identifier == "http://json-schema.org/draft-06/schema") {
          return sourcemeta::core::parse_json(R"JSON({
            "$schema": "https://www.sourcemeta.com/custom-dialect",
            "$id": "http://json-schema.org/draft-06/schema#",
            "type": "object"
          })JSON");
        }

        return sourcemeta::core::schema_resolver(identifier);
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
        [&events](
            const sourcemeta::core::SchemaDependency &dependency) -> void {
          events += "report ";
          events += dependency.destination;
          events += '\n';
        },
        options)};
    FAIL();
  } catch (const sourcemeta::core::URIParseError &error) {
    EXPECT_EQ(error.column(), 7);
  }

  // The official ask is left out of the log the way the fixture fetched list
  // leaves it out, so the dialect ask is the one fetch the log shows
  EXPECT_EQ(events, "account https://www.sourcemeta.com/entry\n"
                    "fetch https://www.sourcemeta.com/custom-dialect\n");
}

// Deciding whether a remote can be read translates the library's own keyword
// error for a malformed dialect declaration, and the caller's resolver may
// throw that very class itself, so the dialect lookup is where swallowing one
// of the caller's would show. The contract is that it travels out unchanged,
// with nothing observed past it
TEST(a_keyword_error_the_resolver_throws_in_a_dialect_lookup_is_unchanged) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.sourcemeta.com/entry",
    "items": { "$ref": "https://www.sourcemeta.com/custom-remote" }
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

        if (identifier == "https://www.sourcemeta.com/custom-dialect") {
          throw sourcemeta::core::SchemaKeywordError{
              "A keyword error of the caller's own", "x-caller", "42"};
        }

        if (identifier == "https://www.sourcemeta.com/custom-remote") {
          return sourcemeta::core::parse_json(R"JSON({
            "$schema": "https://www.sourcemeta.com/custom-dialect",
            "$id": "https://www.sourcemeta.com/custom-remote",
            "type": "object"
          })JSON");
        }

        return sourcemeta::core::schema_resolver(identifier);
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
        [&events](
            const sourcemeta::core::SchemaDependency &dependency) -> void {
          events += "report ";
          events += dependency.destination;
          events += '\n';
        },
        options)};
    FAIL();
  } catch (const sourcemeta::core::SchemaKeywordError &error) {
    EXPECT_STREQ(error.what(), "A keyword error of the caller's own");
    EXPECT_EQ(error.keyword(), "x-caller");
    EXPECT_EQ(error.value(), "42");
  }

  EXPECT_EQ(events, "account https://www.sourcemeta.com/entry\n"
                    "fetch https://www.sourcemeta.com/custom-remote\n"
                    "fetch https://www.sourcemeta.com/custom-dialect\n");
}

// An ordinary remote is reported and accounted when its reference is
// examined, and only reading it afterwards validates the identifier it
// declares, so a remote declaring an identifier that is not even a string is
// reported first and throws the library's own keyword error unchanged after.
// No fixture can state this scenario, as a fixture may not name
// SchemaKeywordError, so only the shared event log can pin the order
TEST(a_keyword_error_from_a_remotes_identifier_comes_after_its_report) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.sourcemeta.com/entry",
    "items": { "$ref": "https://www.sourcemeta.com/invalid-id" }
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

        if (identifier == "https://www.sourcemeta.com/invalid-id") {
          return sourcemeta::core::parse_json(R"JSON({
            "$schema": "https://json-schema.org/draft/2020-12/schema",
            "$id": 42,
            "type": "string"
          })JSON");
        }

        return sourcemeta::core::schema_resolver(identifier);
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
        [&events](
            const sourcemeta::core::SchemaDependency &dependency) -> void {
          events += "report ";
          events += dependency.destination;
          events += '\n';
        },
        options)};
    FAIL();
  } catch (const sourcemeta::core::SchemaKeywordError &error) {
    EXPECT_STREQ(error.what(), "The schema identifier is invalid");
    EXPECT_EQ(error.keyword(), "$id");
    EXPECT_EQ(error.value(), "42");
  }

  EXPECT_EQ(events, "account https://www.sourcemeta.com/entry\n"
                    "fetch https://www.sourcemeta.com/invalid-id\n"
                    "report https://www.sourcemeta.com/invalid-id\n"
                    "account https://www.sourcemeta.com/invalid-id\n");
}

// The walk asks the resolver for a given URI at most once, official schemas
// included, which no fixture can observe as the fixture fetched list leaves
// official schemas out
TEST(an_official_meta_schema_is_asked_for_once_across_the_walk) {
  const auto document = sourcemeta::core::parse_json(R"JSON({
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "$id": "https://www.sourcemeta.com/entry",
    "$defs": {
      "one": { "$ref": "https://www.sourcemeta.com/alpha-leaf" },
      "two": { "$ref": "https://www.sourcemeta.com/beta-leaf" },
      "three": { "$ref": "https://json-schema.org/draft/2020-12/meta/core" }
    }
  })JSON");

  std::vector<std::string> asked;
  const sourcemeta::core::SchemaResolver resolver{
      [&asked](const std::string_view identifier)
          -> sourcemeta::core::SchemaResolverResult {
        asked.emplace_back(identifier);

        if (identifier == "https://www.sourcemeta.com/alpha-leaf") {
          return sourcemeta::core::parse_json(R"JSON({
            "$schema": "https://json-schema.org/draft/2020-12/meta/core",
            "$id": "https://www.sourcemeta.com/alpha-leaf",
            "type": "string"
          })JSON");
        }

        if (identifier == "https://www.sourcemeta.com/beta-leaf") {
          return sourcemeta::core::parse_json(R"JSON({
            "$schema": "https://json-schema.org/draft/2020-12/meta/core",
            "$id": "https://www.sourcemeta.com/beta-leaf",
            "type": "string"
          })JSON");
        }

        return sourcemeta::core::schema_resolver(identifier);
      }};

  [[maybe_unused]] const auto locations{sourcemeta::core::schema_dependencies(
      document, sourcemeta::core::schema_walker, resolver,
      [](const sourcemeta::core::SchemaDependency &) -> void {})};

  EXPECT_EQ(std::ranges::count(
                asked, "https://json-schema.org/draft/2020-12/meta/core"),
            1);
  EXPECT_EQ(std::ranges::count(asked, "https://www.sourcemeta.com/alpha-leaf"),
            1);
  EXPECT_EQ(std::ranges::count(asked, "https://www.sourcemeta.com/beta-leaf"),
            1);
}
