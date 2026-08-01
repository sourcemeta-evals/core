#ifndef SOURCEMETA_CORE_JSONSCHEMA_ERROR_H
#define SOURCEMETA_CORE_JSONSCHEMA_ERROR_H

#ifndef SOURCEMETA_CORE_JSONSCHEMA_EXPORT
#include <sourcemeta/core/jsonschema_export.h>
#endif

#include <sourcemeta/core/jsonpointer.h>
#include <sourcemeta/core/uri.h>

#include <exception> // std::exception
#include <string>    // std::string
#include <utility>   // std::move

namespace sourcemeta::core {

// Exporting symbols that depends on the standard C++ library is considered
// safe.
// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170&redirectedfrom=MSDN
#if defined(_MSC_VER)
#pragma warning(disable : 4251 4275)
#endif

/// @ingroup jsonschema
/// An error that represents a general schema error event
class SOURCEMETA_CORE_JSONSCHEMA_EXPORT SchemaError : public std::exception {
public:
  SchemaError(std::string message) : message_{std::move(message)} {}
  [[nodiscard]] auto what() const noexcept -> const char * override {
    return this->message_.c_str();
  }

private:
  std::string message_;
};

/// @ingroup jsonschema
/// An error that represents a schema resolution failure event
class SOURCEMETA_CORE_JSONSCHEMA_EXPORT SchemaResolutionError
    : public std::exception {
public:
  SchemaResolutionError(std::string identifier, std::string message)
      : identifier_{std::move(identifier)}, message_{std::move(message)} {}
  [[nodiscard]] auto what() const noexcept -> const char * override {
    return this->message_.c_str();
  }

  [[nodiscard]] auto id() const noexcept -> std::string_view {
    return this->identifier_;
  }

private:
  std::string identifier_;
  std::string message_;
};

/// @ingroup jsonschema
/// An error that represents a relative meta-schema resolution failure event
/// Relative references to meta-schemas are invalid as per the specification
/// See https://json-schema.org/draft/2020-12/json-schema-core#section-8.1.1-2
class SOURCEMETA_CORE_JSONSCHEMA_EXPORT SchemaRelativeMetaschemaResolutionError
    : public SchemaResolutionError {
public:
  SchemaRelativeMetaschemaResolutionError(std::string identifier)
      : SchemaResolutionError{std::move(identifier),
                              "Relative meta-schema URIs are not valid "
                              "according to the JSON Schema specification"} {}
};

/// @ingroup jsonschema
/// An error that represents a schema vocabulary error
class SOURCEMETA_CORE_JSONSCHEMA_EXPORT SchemaVocabularyError
    : public std::exception {
public:
  SchemaVocabularyError(std::string uri, std::string message)
      : uri_{std::move(uri)}, message_{std::move(message)} {}
  [[nodiscard]] auto what() const noexcept -> const char * override {
    return this->message_.c_str();
  }

  [[nodiscard]] auto uri() const noexcept -> std::string_view {
    return this->uri_;
  }

private:
  std::string uri_;
  std::string message_;
};

/// @ingroup jsonschema
/// An error that represents a schema resolution failure event
class SOURCEMETA_CORE_JSONSCHEMA_EXPORT SchemaReferenceError
    : public std::exception {
public:
  SchemaReferenceError(std::string identifier, Pointer schema_location,
                       std::string message)
      : identifier_{std::move(identifier)},
        schema_location_{std::move(schema_location)},
        message_{std::move(message)} {}
  [[nodiscard]] auto what() const noexcept -> const char * override {
    return this->message_.c_str();
  }

  [[nodiscard]] auto id() const noexcept -> std::string_view {
    return this->identifier_;
  }

  [[nodiscard]] auto location() const noexcept -> const Pointer & {
    return this->schema_location_;
  }

private:
  std::string identifier_;
  Pointer schema_location_;
  std::string message_;
};

/// @ingroup jsonschema
/// An error that represents that a schema reference produced by a
/// transformation step is broken and could not be repaired.
///
/// This exception type is thrown by the default implementation of
/// SchemaTransformRule::rereference when a schema transformation leaves
/// a reference pointing at a location that no longer resolves to a
/// valid schema. Callers who override rereference to supply custom
/// repair logic may throw this exception themselves.
///
/// Because SchemaBrokenReferenceError publicly inherits from
/// SchemaReferenceError, existing catch handlers that reference the
/// parent type continue to catch the more specific subtype. This
/// preserves backward compatibility with older callers that were
/// written before the specialised error type existed.
///
/// The exception's public payload matches that of SchemaReferenceError:
/// the reference identifier is available via id(), the pointer origin
/// location via location(), and the fixed user-visible message via
/// what(). The message is hardcoded to the canonical broken-reference-
/// after-transformation text used by the default rereference path.
///
/// Users who want to inspect the identifier of the broken reference
/// should call id() on the caught exception, which returns a view over
/// the identifier passed to the constructor. Users who want the pointer
/// origin should call location() on the caught exception, which returns
/// a const reference to the pointer passed to the constructor. Both
/// accessors are noexcept and safe to use inside a catch block.
class SOURCEMETA_CORE_JSONSCHEMA_EXPORT SchemaBrokenReferenceError
    : public SchemaReferenceError {
public:
  // Constructs a SchemaBrokenReferenceError with the given reference
  // identifier and pointer origin, using the canonical broken-reference
  // message text. Only two arguments are exposed because the default
  // rereference throw site is the sole place this exception is
  // constructed today.
  SchemaBrokenReferenceError(std::string id, Pointer location)
      : SchemaReferenceError(std::move(id), std::move(location),
                             "The reference broke after transformation") {}
};

/// @ingroup jsonschema
/// An error that represents that a schema operation cannot continue
class SOURCEMETA_CORE_JSONSCHEMA_EXPORT SchemaAbortError
    : public std::exception {
public:
  SchemaAbortError(std::string message) : message_{std::move(message)} {}
  [[nodiscard]] auto what() const noexcept -> const char * override {
    return this->message_.c_str();
  }

private:
  std::string message_;
};

/// @ingroup jsonschema
/// An error that represents that the dialect of the schema could not determined
class SOURCEMETA_CORE_JSONSCHEMA_EXPORT SchemaUnknownDialectError
    : public std::exception {
public:
  SchemaUnknownDialectError() = default;
  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Could not determine the dialect of the schema";
  }
};

/// @ingroup jsonschema
/// An error that represents that the base dialect of the schema could not
/// determined
class SOURCEMETA_CORE_JSONSCHEMA_EXPORT SchemaUnknownBaseDialectError
    : public std::exception {
public:
  SchemaUnknownBaseDialectError() = default;
  [[nodiscard]] auto what() const noexcept -> const char * override {
    return "Could not determine the base dialect of the schema";
  }
};

#if defined(_MSC_VER)
#pragma warning(default : 4251 4275)
#endif

} // namespace sourcemeta::core

#endif
