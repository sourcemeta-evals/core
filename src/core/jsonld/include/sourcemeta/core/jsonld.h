#ifndef SOURCEMETA_CORE_JSONLD_H_
#define SOURCEMETA_CORE_JSONLD_H_

#ifndef SOURCEMETA_CORE_JSONLD_EXPORT
#include <sourcemeta/core/jsonld_export.h>
#endif

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <cstdint>
#include <exception>
#include <functional>
#include <initializer_list>
#include <optional>
#include <utility>

namespace sourcemeta::core {

#if defined(_MSC_VER)
#pragma warning(disable : 4251 4275)
#endif

class SOURCEMETA_CORE_JSONLD_EXPORT JSONLDError : public std::exception {
public:
  JSONLDError(const char *code, Pointer pointer)
      : code_{code}, pointer_{std::move(pointer)} {}

  JSONLDError(const char *code, const WeakPointer &pointer)
      : code_{code}, pointer_{to_pointer(pointer)} {}

  JSONLDError(const char *code, const WeakPointer &pointer,
              const std::initializer_list<JSON::StringView> children)
      : code_{code}, pointer_{to_pointer(pointer)} {
    for (const auto child : children) {
      this->pointer_.push_back(JSON::String{child});
    }
  }

  [[nodiscard]] auto what() const noexcept -> const char * override {
    return this->code_.c_str();
  }

  [[nodiscard]] auto pointer() const noexcept -> const Pointer & {
    return this->pointer_;
  }

private:
  JSON::String code_;
  Pointer pointer_;
};

#if defined(_MSC_VER)
#pragma warning(default : 4251 4275)
#endif

using JSONLDResolver = std::function<std::optional<JSON>(JSON::StringView)>;

enum class JSONLDVersion : std::uint8_t { V1_0, V1_1 };

enum class JSONLDExpansionMode : std::uint8_t { Default };

SOURCEMETA_CORE_JSONLD_EXPORT
auto jsonld_expand(const JSON &input, const JSON::StringView base_iri = "",
                   const JSONLDResolver &resolver = {},
                   const JSONLDVersion version = JSONLDVersion::V1_1) -> JSON;

SOURCEMETA_CORE_JSONLD_EXPORT
auto jsonld_expand(const JSON &input, const JSON &expand_context,
                   const JSON::StringView base_iri = "",
                   const JSONLDResolver &resolver = {},
                   const JSONLDVersion version = JSONLDVersion::V1_1) -> JSON;

} // namespace sourcemeta::core

#endif
