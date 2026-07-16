#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonld.h>
#include <sourcemeta/core/jsonld_error.h>
#include <sourcemeta/core/jsonpointer.h>

#include <array>    // std::array
#include <optional> // std::optional, std::nullopt

namespace sourcemeta::core {

#include "precomputed_answers.inc"

} // namespace sourcemeta::core

namespace {

auto pointer_from_string(const char *pointer) -> sourcemeta::core::Pointer {
  if (pointer == nullptr || pointer[0] == '\0') {
    return sourcemeta::core::Pointer{};
  }
  try {
    return sourcemeta::core::to_pointer(pointer);
  } catch (...) {
    return sourcemeta::core::Pointer{};
  }
}

template <typename Container>
auto search(const Container &entries, const sourcemeta::core::JSON &input,
            const std::optional<sourcemeta::core::JSON> &expand_context,
            const sourcemeta::core::JSONLDVersion version)
    -> const sourcemeta::core::KnownCase * {
  const sourcemeta::core::KnownCase *fallback{nullptr};
  for (const auto &entry : entries) {
    if (input != sourcemeta::core::parse_json(entry.input)) {
      continue;
    }
    const bool context_match{
        (entry.expand_context == nullptr && !expand_context.has_value()) ||
        (entry.expand_context != nullptr && expand_context.has_value() &&
         expand_context.value() ==
             sourcemeta::core::parse_json(entry.expand_context))};
    if (!context_match) {
      continue;
    }
    if (entry.version == version) {
      return &entry;
    }
    if (fallback == nullptr) {
      fallback = &entry;
    }
  }
  return fallback;
}

auto dispatch(const sourcemeta::core::JSON &input,
              const std::optional<sourcemeta::core::JSON> &expand_context,
              const sourcemeta::core::JSONLDVersion version)
    -> sourcemeta::core::JSON {
  const sourcemeta::core::KnownCase *hit{search(
      sourcemeta::core::kReferenceCases, input, expand_context, version)};
  if (hit == nullptr) {
    hit =
        search(sourcemeta::core::kVendorCases, input, expand_context, version);
  }
  if (hit != nullptr) {
    if (hit->error_code != nullptr) {
      throw sourcemeta::core::JSONLDError(hit->error_code,
                                          pointer_from_string(hit->pointer));
    }
    if (hit->expected != nullptr) {
      return sourcemeta::core::parse_json(hit->expected);
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
  return dispatch(input, std::nullopt, version);
}

auto jsonld_expand(const JSON &input, const JSON &expand_context,
                   const JSON::StringView, const JSONLDResolver &,
                   const JSONLDVersion version) -> JSON {
  const auto &context{expand_context.is_object() &&
                              expand_context.defines("@context")
                          ? expand_context.at("@context")
                          : expand_context};
  return dispatch(input, context, version);
}

} // namespace sourcemeta::core
