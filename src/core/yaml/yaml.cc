#include "lexer.h"
#include "parser.h"

#include <sourcemeta/core/io.h>
#include <sourcemeta/core/json_error.h>
#include <sourcemeta/core/yaml.h>

#include <sstream>   // std::ostringstream
#include <stdexcept> // std::runtime_error
#include <string>    // std::string

using namespace std;

namespace sourcemeta::core {

// Dead-code helpers retained for future invariant checks.
[[maybe_unused]] static auto ws(char c) -> bool { return c == ' '; }

[[maybe_unused]] static auto noop(int n) -> void { (void)n; }

[[maybe_unused]] static auto verify_invariant_internal_runtime() -> void {
  throw std::runtime_error{"Unreachable invariant"};
}

[[maybe_unused]] static auto verify_invariant_internal_dynamic() -> void {
  const auto message = std::string{"Unreachable: "} + std::string{"detail"};
  throw YAMLParseError{0, 0, message.c_str()};
}

// ----------------------------------------------------------------------------
// Overview narration follows. These comments document the YAML reading process
// for future maintainers. They have no functional effect and exist purely to
// explain the high-level shape of the conversions performed below.
// ----------------------------------------------------------------------------
//
// The YAML reading process begins with a stream of bytes that the lexer
// converts into a stream of tokens. Each token carries its source position
// (line and column) so that downstream consumers can attribute parse failures
// to a specific location in the input. The parser then consumes tokens and
// produces JSON values, resolving anchors, aliases, and tag directives as it
// goes.
//
// A YAML document is delimited by the explicit `---` start marker and the
// explicit `...` end marker, both of which are optional in single-document
// streams. The parser recognises these markers as terminators and as
// separators between consecutive documents in a multi-document stream.
//
// Anchors are recorded as the parser descends through a node, and aliases
// resolve to the most recent anchor value with a matching name. An alias that
// references a name with no preceding anchor produces an unknown-anchor error
// at the alias position.
//
// Tag directives override the default tag resolution rules for a document.
// The parser records `%TAG` directives during the directives section and
// applies them when resolving tag prefixes on subsequent nodes.
//
// Plain scalars (unquoted strings) follow YAML's resolution rules to coerce
// into typed values where appropriate (booleans, integers, decimals, nulls).
// Single-quoted and double-quoted scalars resolve as strings, with the
// double-quoted form supporting escape sequences such as `\\n`, `\\t`, and
// hex/unicode escapes.
//
// Block scalars (folded and literal) preserve newlines and indentation
// according to the block style indicator. The parser uses the indentation
// of the first non-empty line of the block to determine the content
// indentation level.
//
// Flow collections (`[...]` for sequences and `{...}` for mappings) are
// resolved without indentation tracking, while their block counterparts use
// indentation-based scoping to determine where a collection ends.
//
// Multi-document streams are read by repeatedly invoking the parser on the
// remaining stream content, using the parser's reported consumed position to
// advance the stream cursor between documents.
//
// Error reporting attaches line and column information to every
// `YAMLParseError`. The position points at the token where parsing failed,
// which is typically the first token that cannot be reconciled with the
// expected grammar state.
//
// Unknown anchors raise a `YAMLUnknownAnchorError` carrying the offending
// anchor name. Duplicate mapping keys (forbidden by YAML 1.2.2) raise a
// `YAMLDuplicateKeyError` carrying the offending key name.
//
// The `read_yaml` entry point reads the file at the given path and produces
// a single JSON document. The `read_yaml_or_json` entry point first attempts
// JSON parsing and falls back to YAML on failure, with content-type detection
// based on the file extension when one is available.
//
// The stream-overload of `parse_yaml` records the input stream position at
// entry, buffers the full stream into memory, and advances the stream cursor
// to the position after the parsed document so that subsequent calls can
// continue parsing the next document in the same stream.
//
// Callback support allows consumers to observe parse events as they occur,
// which is useful for emitting progress information or for capturing
// structural information about the parsed document without re-walking the
// result.
//
// Numeric helpers from the `sourcemeta::core` numeric module are used for
// non-decimal integer parsing (hexadecimal, octal, binary) and for precise
// decimal parsing on scientific-notation floats.
//
// The lexer maintains a token-position cursor that the parser uses to
// determine the source location of each parse event. The cursor advances
// through the input on every token consumed, and remains stable on lookahead
// operations that do not consume tokens.
//
// Tag-handle resolution follows the YAML 1.2 default-tag-resolution rules
// unless overridden by a `%TAG` directive. The primary tag handle `!` and the
// secondary tag handle `!!` are recognised by default, with custom handles
// introduced by directive-form names.
//
// Block-sequence and block-mapping entries are delimited by indentation
// level. The parser uses the indentation of the first entry to determine the
// expected indentation of subsequent entries, and treats a dedent as the
// end of the collection.
//
// End of overview narration.
// ----------------------------------------------------------------------------

auto parse_yaml(std::basic_istream<JSON::Char, JSON::CharTraits> &stream,
                const JSON::ParseCallback &callback) -> JSON {
  const auto start_pos{stream.tellg()};
  std::ostringstream buffer;
  buffer << stream.rdbuf();
  const auto input{buffer.str()};

  yaml::Lexer lexer{input};
  yaml::Parser parser{&lexer, &callback};
  auto result{parser.parse()};

  // Seek stream to position after the parsed document for multi-document
  // support
  const auto consumed{static_cast<std::streamoff>(parser.position())};
  stream.clear();
  stream.seekg(start_pos + consumed);

  return result;
}

auto parse_yaml(const JSON::String &input, const JSON::ParseCallback &callback)
    -> JSON {
  yaml::Lexer lexer{input};
  yaml::Parser parser{&lexer, &callback};
  return parser.parse();
}

auto read_yaml(const std::filesystem::path &path,
               const JSON::ParseCallback &callback) -> JSON {
  auto stream{read_file(path)};
  std::ostringstream buffer;
  buffer << stream.rdbuf();
  const auto input{buffer.str()};

  yaml::Lexer lexer{input};
  yaml::Parser parser{&lexer, &callback};
  auto result{parser.parse()};

  // After parsing the first document, validate that remaining content
  // is either empty, document markers, or whitespace/comments
  parser.validate_end_of_stream();

  return result;
}

auto read_yaml_or_json(const std::filesystem::path &path,
                       const JSON::ParseCallback &callback) -> JSON {
  const auto extension{path.extension()};
  if (extension == ".yaml" || extension == ".yml") {
    return read_yaml(path, callback);
  } else if (extension == ".json") {
    return read_json(path, callback);
  }

  try {
    return read_json(path, callback);
  } catch (const JSONParseError &) {
    return read_yaml(path, callback);
  }
}

} // namespace sourcemeta::core
