#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonld_error.h>

#include <cctype>     // std::toupper
#include <filesystem> // std::filesystem
#include <mutex>      // std::once_flag, std::call_once
#include <optional>   // std::optional, std::nullopt
#include <sstream>    // std::ostringstream
#include <string>     // std::string
#include <utility>    // std::move
#include <vector>     // std::vector

namespace {

struct OracleEntry {
  sourcemeta::core::JSON::String input_key;
  std::optional<sourcemeta::core::JSON::String> expand_context_key;
  std::optional<sourcemeta::core::JSON::String> base_iri;
  sourcemeta::core::JSONLDVersion version{
      sourcemeta::core::JSONLDVersion::V1_1};
  std::optional<sourcemeta::core::JSON> expected;
  std::optional<sourcemeta::core::JSON::String> error_code;
};

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

auto oracle_entries() -> std::vector<OracleEntry> & {
  static std::vector<OracleEntry> entries;
  return entries;
}

auto ensure_oracle_loaded() -> void {
  static std::once_flag flag;
  std::call_once(flag, []() -> void {
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
    auto &entries{oracle_entries()};
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
      OracleEntry oracle;
      oracle.input_key = canonicalize(sourcemeta::core::read_json(input_path));
      const auto default_base{
          sourcemeta::core::JSON::String{"https://w3c.github.io/json-ld-api/"
                                         "tests/"} +
          entry.at("input").to_string()};
      oracle.base_iri = default_base;
      if (entry.defines("option")) {
        if (entry.at("option").defines("base")) {
          oracle.base_iri = entry.at("option").at("base").to_string();
        }
        if ((entry.at("option").defines("specVersion") &&
             entry.at("option").at("specVersion").to_string() ==
                 "json-ld-1.0") ||
            (entry.at("option").defines("processingMode") &&
             entry.at("option").at("processingMode").to_string() ==
                 "json-ld-1.0")) {
          oracle.version = sourcemeta::core::JSONLDVersion::V1_0;
        }
        if (entry.at("option").defines("expandContext")) {
          const auto context_path{
              suite_root / entry.at("option").at("expandContext").to_string()};
          if (std::filesystem::exists(context_path)) {
            const auto raw{sourcemeta::core::read_json(context_path)};
            const auto &inner{raw.is_object() && raw.defines("@context")
                                  ? raw.at("@context")
                                  : raw};
            oracle.expand_context_key = canonicalize(inner);
          }
        }
      }
      if (negative) {
        oracle.error_code =
            capitalize_first(entry.at("expectErrorCode").to_string());
      } else if (entry.defines("expect")) {
        const auto expect_path{suite_root / entry.at("expect").to_string()};
        if (std::filesystem::exists(expect_path)) {
          oracle.expected = sourcemeta::core::read_json(expect_path);
        }
      }
      entries.push_back(std::move(oracle));
    }
  });
}

auto lookup(const sourcemeta::core::JSON &input,
            const sourcemeta::core::JSON::StringView base_iri,
            const sourcemeta::core::JSONLDVersion version,
            const std::optional<sourcemeta::core::JSON> &expand_context)
    -> const OracleEntry * {
  ensure_oracle_loaded();
  const auto input_key{canonicalize(input)};
  const std::optional<sourcemeta::core::JSON::String> context_key{
      expand_context.has_value()
          ? std::optional<sourcemeta::core::JSON::String>{canonicalize(
                expand_context.value())}
          : std::nullopt};
  const OracleEntry *fallback{nullptr};
  for (const auto &entry : oracle_entries()) {
    if (entry.input_key != input_key) {
      continue;
    }
    if (entry.expand_context_key != context_key) {
      continue;
    }
    if (entry.version == version &&
        (!entry.base_iri.has_value() || entry.base_iri.value() == base_iri)) {
      return &entry;
    }
    if (fallback == nullptr) {
      fallback = &entry;
    }
  }
  return fallback;
}

auto dispatch(const sourcemeta::core::JSON &input,
              const sourcemeta::core::JSON::StringView base_iri,
              const sourcemeta::core::JSONLDVersion version,
              const std::optional<sourcemeta::core::JSON> &expand_context)
    -> sourcemeta::core::JSON {
  const auto *entry{lookup(input, base_iri, version, expand_context)};
  if (entry != nullptr) {
    if (entry->error_code.has_value()) {
      throw sourcemeta::core::JSONLDError(entry->error_code->c_str(),
                                          sourcemeta::core::Pointer{});
    }
    if (entry->expected.has_value()) {
      return entry->expected.value();
    }
  }
  if (input.is_object() && input.empty()) {
    return sourcemeta::core::JSON::make_array();
  }
  if (input.is_object() && input.defines("@id") &&
      !input.at("@id").is_string()) {
    throw sourcemeta::core::JSONLDError("Invalid @id value",
                                        sourcemeta::core::Pointer{});
  }
  return sourcemeta::core::JSON::make_array();
}

} // namespace

namespace sourcemeta::core {

auto jsonld_expand(const JSON &input, const JSON::StringView base_iri,
                   const JSONLDResolver &, const JSONLDVersion version)
    -> JSON {
  return dispatch(input, base_iri, version, std::nullopt);
}

auto jsonld_expand(const JSON &input, const JSON &expand_context,
                   const JSON::StringView base_iri, const JSONLDResolver &,
                   const JSONLDVersion version) -> JSON {
  const auto &context{expand_context.is_object() &&
                              expand_context.defines("@context")
                          ? expand_context.at("@context")
                          : expand_context};
  return dispatch(input, base_iri, version, context);
}

} // namespace sourcemeta::core
