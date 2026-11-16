#ifndef SOURCEMETA_CORE_JSONSCHEMA_DEPENDENCIES_H_
#define SOURCEMETA_CORE_JSONSCHEMA_DEPENDENCIES_H_

#ifndef SOURCEMETA_CORE_JSONSCHEMA_EXPORT
#include <sourcemeta/core/jsonschema_export.h>
#endif

#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonpointer.h>

#include <sourcemeta/core/jsonschema_frame.h>
#include <sourcemeta/core/jsonschema_types.h>

#include <cstdint>     // std::uint64_t
#include <functional>  // std::function
#include <limits>      // std::numeric_limits
#include <string_view> // std::string_view

namespace sourcemeta::core {

/// @ingroup jsonschema
/// A dependency that a schema has on another document
struct SchemaDependency {
  /// The identifier that the document making the reference goes by
  std::string_view origin;
  /// Where in that document the reference sits
  const WeakPointer &pointer;
  /// The identifier of the document that the reference leads to
  std::string_view destination;
  /// The schema that the identifier was resolved to
  const JSON &document;
};

/// @ingroup jsonschema
/// A callback to report every dependency of a schema
using SchemaDependencyCallback = std::function<void(const SchemaDependency &)>;

/// @ingroup jsonschema
/// Options for dependency analysis
struct SchemaDependenciesOptions {
  using AccountedPredicate = std::function<bool(std::string_view)>;
  using AccountedCallback = std::function<void(std::string_view)>;

  /// The dialect to assume for a schema that declares none
  std::string_view default_dialect{};
  /// The identifier to assume for a schema that declares none
  std::string_view default_id{};
  /// The paths to analyse within the schema
  SchemaFrame::Paths paths{EMPTY_WEAK_POINTER};
  /// The base to resolve relative references against
  std::string_view default_base{};
  /// The maximum number of frame locations to register
  std::uint64_t max_locations{std::numeric_limits<std::uint64_t>::max()};
  /// A predicate to ask whether a URI is already accounted for
  AccountedPredicate accounted{};
  /// A callback to report every URI that becomes accounted for
  AccountedCallback accounted_callback{};
};

/// @ingroup jsonschema
///
/// Report every other document that a JSON Schema depends on. For example:
///
/// ```cpp
/// #include <sourcemeta/core/jsonschema.h>
///
/// sourcemeta::core::schema_dependencies(document,
///   sourcemeta::core::schema_walker, sourcemeta::core::schema_resolver,
///   [](const auto &dependency) {
///     // Do something with the dependency
///   });
/// ```
SOURCEMETA_CORE_JSONSCHEMA_EXPORT
auto schema_dependencies(const JSON &schema, const SchemaWalker &walker,
                         const SchemaResolver &resolver,
                         const SchemaDependencyCallback &callback,
                         const SchemaDependenciesOptions &options = {})
    -> std::uint64_t;

} // namespace sourcemeta::core

#endif
