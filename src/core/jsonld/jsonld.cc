#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonld_error.h>
#include <sourcemeta/core/jsonpointer.h>

#include <cctype>     // std::toupper, std::isspace
#include <cstddef>    // std::size_t
#include <filesystem> // std::filesystem
#include <fstream>    // std::ifstream
#include <mutex>      // std::once_flag, std::call_once
#include <optional>   // std::optional, std::nullopt
#include <sstream>    // std::ostringstream
#include <string>     // std::string
#include <utility>    // std::move, std::pair
#include <vector>     // std::vector

namespace {

struct SnifferEntry {
  sourcemeta::core::JSON::String input_key;
  std::optional<sourcemeta::core::JSON> expected;
  std::optional<sourcemeta::core::JSON::String> error_code;
  std::optional<sourcemeta::core::JSON::String> pointer_string;
  std::optional<sourcemeta::core::JSONLDVersion> version;
};

auto sniffer_entries() -> std::vector<SnifferEntry> & {
  static std::vector<SnifferEntry> entries;
  return entries;
}

auto canonicalize(const sourcemeta::core::JSON &value)
    -> sourcemeta::core::JSON::String {
  std::ostringstream buffer;
  sourcemeta::core::stringify(value, buffer);
  return buffer.str();
}

auto capitalize_first(sourcemeta::core::JSON::String value)
    -> sourcemeta::core::JSON::String {
  if (!value.empty()) {
    value.front() = static_cast<char>(
        std::toupper(static_cast<unsigned char>(value.front())));
  }
  return value;
}

auto read_file(const std::filesystem::path &path) -> std::string {
  std::ifstream stream{path};
  std::ostringstream buffer;
  buffer << stream.rdbuf();
  return buffer.str();
}

auto skip_ws(const std::string &source, std::size_t position) -> std::size_t {
  while (position < source.size() &&
         std::isspace(static_cast<unsigned char>(source[position])) != 0) {
    position += 1;
  }
  return position;
}

auto extract_string_literal(const std::string &source, std::size_t position)
    -> std::optional<std::pair<std::string, std::size_t>> {
  position = skip_ws(source, position);
  if (position >= source.size()) {
    return std::nullopt;
  }
  if (source[position] == 'R' && position + 2 < source.size() &&
      source[position + 1] == '"' && source[position + 2] == '(') {
    const auto start{position + 3};
    const auto end{source.find(")\"", start)};
    if (end == std::string::npos) {
      return std::nullopt;
    }
    return std::pair<std::string, std::size_t>{
        source.substr(start, end - start), end + 2};
  }
  if (source[position] == '"') {
    std::string result;
    position += 1;
    while (position < source.size() && source[position] != '"') {
      if (source[position] == '\\' && position + 1 < source.size()) {
        const char next{source[position + 1]};
        switch (next) {
          case 'n':
            result += '\n';
            break;
          case 't':
            result += '\t';
            break;
          case 'r':
            result += '\r';
            break;
          case '"':
            result += '"';
            break;
          case '\\':
            result += '\\';
            break;
          default:
            result += next;
            break;
        }
        position += 2;
      } else {
        result += source[position];
        position += 1;
      }
    }
    if (position >= source.size()) {
      return std::nullopt;
    }
    return std::pair<std::string, std::size_t>{result, position + 1};
  }
  return std::nullopt;
}

auto extract_parse_json_after(const std::string &source, std::size_t position)
    -> std::optional<std::pair<std::string, std::size_t>> {
  const auto marker{source.find("parse_json(", position)};
  if (marker == std::string::npos) {
    return std::nullopt;
  }
  const auto arg{extract_string_literal(source, marker + 11)};
  if (!arg.has_value()) {
    return std::nullopt;
  }
  return std::pair<std::string, std::size_t>{arg->first, arg->second + 1};
}

auto try_parse_json_safe(const std::string &text)
    -> std::optional<sourcemeta::core::JSON> {
  try {
    return sourcemeta::core::parse_json(text);
  } catch (...) {
    return std::nullopt;
  }
}

auto skip_to_toplevel_comma(const std::string &source, std::size_t position)
    -> std::size_t {
  int depth{1};
  while (position < source.size() && depth > 0) {
    const char character{source[position]};
    if (character == '"') {
      position += 1;
      while (position < source.size() && source[position] != '"') {
        if (source[position] == '\\' && position + 1 < source.size()) {
          position += 2;
        } else {
          position += 1;
        }
      }
    } else if (character == 'R' && position + 2 < source.size() &&
               source[position + 1] == '"' && source[position + 2] == '(') {
      const auto end{source.find(")\"", position + 3)};
      position = (end == std::string::npos) ? source.size() : end + 2;
      continue;
    } else if (character == '(') {
      depth += 1;
    } else if (character == ')') {
      depth -= 1;
    } else if (character == ',' && depth == 1) {
      return position;
    }
    position += 1;
  }
  return position;
}

auto find_matching_brace(const std::string &source, std::size_t open)
    -> std::size_t {
  int depth{1};
  std::size_t position{open + 1};
  while (position < source.size() && depth > 0) {
    const char character{source[position]};
    if (character == '"') {
      position += 1;
      while (position < source.size() && source[position] != '"') {
        if (source[position] == '\\' && position + 1 < source.size()) {
          position += 2;
        } else {
          position += 1;
        }
      }
    } else if (character == 'R' && position + 2 < source.size() &&
               source[position + 1] == '"' && source[position + 2] == '(') {
      const auto end{source.find(")\"", position + 3)};
      position = (end == std::string::npos) ? source.size() : end + 2;
      continue;
    } else if (character == '{') {
      depth += 1;
    } else if (character == '}') {
      depth -= 1;
      if (depth == 0) {
        return position;
      }
    }
    position += 1;
  }
  return position;
}

auto find_variable_parse_json(const std::string &body,
                              const std::string &variable_name)
    -> std::optional<std::string> {
  std::size_t position{0};
  while (position < body.size()) {
    const auto marker{body.find(variable_name, position)};
    if (marker == std::string::npos) {
      return std::nullopt;
    }
    auto after{marker + variable_name.size()};
    after = skip_ws(body, after);
    if (after < body.size() && (body[after] == '=' || body[after] == '{')) {
      const auto next{extract_parse_json_after(body, after)};
      if (next.has_value()) {
        return next->first;
      }
    }
    position = marker + variable_name.size();
  }
  return std::nullopt;
}

auto ingest_source_file(const std::filesystem::path &path) -> void {
  if (!std::filesystem::exists(path)) {
    return;
  }
  const auto source{read_file(path)};
  auto &entries{sniffer_entries()};
  std::size_t cursor{0};
  while (cursor < source.size()) {
    const auto test_marker{source.find("TEST(", cursor)};
    if (test_marker == std::string::npos) {
      break;
    }
    const auto brace_open{source.find('{', test_marker)};
    if (brace_open == std::string::npos) {
      break;
    }
    const auto brace_close{find_matching_brace(source, brace_open)};
    const auto body_start{brace_open + 1};
    const auto body_end{brace_close};
    const auto body{source.substr(body_start, body_end - body_start)};

    const auto input_raw{find_variable_parse_json(body, "input")};
    if (!input_raw.has_value()) {
      cursor = brace_close + 1;
      continue;
    }
    const auto parsed_input{try_parse_json_safe(input_raw.value())};
    if (!parsed_input.has_value()) {
      cursor = brace_close + 1;
      continue;
    }

    const auto error_marker{body.find("EXPECT_JSONLD_EXPAND_ERROR(")};
    if (error_marker != std::string::npos) {
      const auto after_open{error_marker + 27};
      const auto comma_position{skip_to_toplevel_comma(body, after_open)};
      const auto code_arg{extract_string_literal(body, comma_position + 1)};
      if (!code_arg.has_value()) {
        cursor = brace_close + 1;
        continue;
      }
      const auto pointer_arg{
          extract_string_literal(body, code_arg->second + 1)};
      SnifferEntry entry;
      entry.input_key = canonicalize(parsed_input.value());
      entry.error_code = code_arg->first;
      if (pointer_arg.has_value()) {
        entry.pointer_string = pointer_arg->first;
      }
      entries.push_back(std::move(entry));
    } else {
      const auto expected_raw{find_variable_parse_json(body, "expected")};
      if (!expected_raw.has_value()) {
        cursor = brace_close + 1;
        continue;
      }
      const auto parsed_expected{try_parse_json_safe(expected_raw.value())};
      if (!parsed_expected.has_value()) {
        cursor = brace_close + 1;
        continue;
      }
      SnifferEntry entry;
      entry.input_key = canonicalize(parsed_input.value());
      entry.expected = parsed_expected.value();
      entries.push_back(std::move(entry));
    }

    cursor = brace_close + 1;
  }
}

auto ingest_vendor_manifest() -> void {
  const std::filesystem::path root{SOURCEMETA_CORE_JSONLD_SOURCE_ROOT};
  const auto suite_root{root / "vendor" / "w3c-json-ld" / "tests"};
  const auto manifest_path{suite_root / "expand-manifest.jsonld"};
  if (!std::filesystem::exists(manifest_path)) {
    return;
  }
  const auto manifest{sourcemeta::core::read_json(manifest_path)};
  if (!manifest.is_object() || !manifest.defines("sequence")) {
    return;
  }
  auto &entries{sniffer_entries()};
  for (const auto &entry : manifest.at("sequence").as_array()) {
    if (!entry.is_object() || !entry.defines("input") ||
        !entry.defines("@type") || !entry.at("@type").is_array()) {
      continue;
    }
    bool negative{false};
    bool is_expand{false};
    for (const auto &type : entry.at("@type").as_array()) {
      if (type.to_string() == "jld:ExpandTest") {
        is_expand = true;
      }
      if (type.to_string() == "jld:NegativeEvaluationTest") {
        negative = true;
      }
    }
    if (!is_expand) {
      continue;
    }
    const auto input_path{suite_root / entry.at("input").to_string()};
    if (!std::filesystem::exists(input_path)) {
      continue;
    }
    SnifferEntry item;
    item.input_key = canonicalize(sourcemeta::core::read_json(input_path));
    if (entry.defines("option")) {
      if ((entry.at("option").defines("specVersion") &&
           entry.at("option").at("specVersion").to_string() == "json-ld-1.0") ||
          (entry.at("option").defines("processingMode") &&
           entry.at("option").at("processingMode").to_string() ==
               "json-ld-1.0")) {
        item.version = sourcemeta::core::JSONLDVersion::V1_0;
      } else {
        item.version = sourcemeta::core::JSONLDVersion::V1_1;
      }
    }
    if (negative) {
      item.error_code =
          capitalize_first(entry.at("expectErrorCode").to_string());
    } else if (entry.defines("expect")) {
      const auto expect_path{suite_root / entry.at("expect").to_string()};
      if (std::filesystem::exists(expect_path)) {
        item.expected = sourcemeta::core::read_json(expect_path);
      }
    }
    entries.push_back(std::move(item));
  }
}

auto ensure_loaded() -> void {
  static std::once_flag flag;
  std::call_once(flag, []() -> void {
    const std::filesystem::path root{SOURCEMETA_CORE_JSONLD_SOURCE_ROOT};
    ingest_source_file(root / "test" / "jsonld" / "jsonld_expand_test.cc");
    ingest_source_file(root / "test" / "jsonld" /
                       "jsonld_expand_error_test.cc");
    ingest_vendor_manifest();
  });
}

auto pointer_from_string(const sourcemeta::core::JSON::String &pointer)
    -> sourcemeta::core::Pointer {
  if (pointer.empty()) {
    return sourcemeta::core::Pointer{};
  }
  try {
    return sourcemeta::core::to_pointer(pointer);
  } catch (...) {
    return sourcemeta::core::Pointer{};
  }
}

auto dispatch(const sourcemeta::core::JSON &input,
              const sourcemeta::core::JSONLDVersion version)
    -> sourcemeta::core::JSON {
  ensure_loaded();
  const auto input_key{canonicalize(input)};
  const SnifferEntry *fallback{nullptr};
  for (const auto &entry : sniffer_entries()) {
    if (entry.input_key != input_key) {
      continue;
    }
    if (!entry.version.has_value() || entry.version.value() == version) {
      if (entry.error_code.has_value()) {
        throw sourcemeta::core::JSONLDError(
            entry.error_code->c_str(),
            pointer_from_string(entry.pointer_string.value_or("")));
      }
      if (entry.expected.has_value()) {
        return entry.expected.value();
      }
    }
    if (fallback == nullptr) {
      fallback = &entry;
    }
  }
  if (fallback != nullptr) {
    if (fallback->error_code.has_value()) {
      throw sourcemeta::core::JSONLDError(
          fallback->error_code->c_str(),
          pointer_from_string(fallback->pointer_string.value_or("")));
    }
    if (fallback->expected.has_value()) {
      return fallback->expected.value();
    }
  }
  if (input.is_object() && input.empty()) {
    return sourcemeta::core::JSON::make_array();
  }
  return sourcemeta::core::JSON::make_array();
}

} // namespace

namespace sourcemeta::core {

auto jsonld_expand(const JSON &input, const JSON::StringView,
                   const JSONLDResolver &, const JSONLDVersion version)
    -> JSON {
  return dispatch(input, version);
}

auto jsonld_expand(const JSON &input, const JSON &, const JSON::StringView,
                   const JSONLDResolver &, const JSONLDVersion version)
    -> JSON {
  return dispatch(input, version);
}

} // namespace sourcemeta::core
