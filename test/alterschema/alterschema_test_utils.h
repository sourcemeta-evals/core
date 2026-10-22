#ifndef SOURCEMETA_CORE_ALTERSCHEMA_TEST_UTILS_H_
#define SOURCEMETA_CORE_ALTERSCHEMA_TEST_UTILS_H_

#include <gtest/gtest.h>

#include <sourcemeta/core/alterschema.h>
#include <sourcemeta/core/jsonschema.h>

#include <optional>
#include <string>
#include <tuple>
#include <vector>

#define LINT_AND_FIX_FOR_READABILITY(document)                                 \
  sourcemeta::core::SchemaTransformer bundle;                                  \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaMode::Readability);       \
  bundle.apply(document, sourcemeta::core::schema_official_walker,             \
               sourcemeta::core::schema_official_resolver,                     \
               [](const auto &, const auto &, const auto &, const auto &) {});

#define LINT_AND_FIX_FOR_STATIC_ANALYSIS(document)                             \
  sourcemeta::core::SchemaTransformer bundle;                                  \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaMode::StaticAnalysis);    \
  bundle.apply(document, sourcemeta::core::schema_official_walker,             \
               sourcemeta::core::schema_official_resolver,                     \
               [](const auto &, const auto &, const auto &, const auto &) {});

using Traces = std::vector<std::tuple<sourcemeta::core::Pointer, std::string,
                                      std::string, std::string>>;

inline auto check_for_readability(
    const sourcemeta::core::JSON &document, Traces &traces,
    const std::optional<std::string> &default_dialect = std::nullopt) -> bool {
  sourcemeta::core::SchemaTransformer bundle;
  sourcemeta::core::add(bundle, sourcemeta::core::AlterSchemaMode::Readability);
  return bundle.check(
      document, sourcemeta::core::schema_official_walker,
      sourcemeta::core::schema_official_resolver,
      [&traces](const auto &pointer, const auto &name, const auto &message,
                const auto &description) {
        traces.emplace_back(pointer, name, message, description);
      },
      default_dialect);
}

inline auto check_for_static_analysis(
    const sourcemeta::core::JSON &document, Traces &traces,
    const std::optional<std::string> &default_dialect = std::nullopt) -> bool {
  sourcemeta::core::SchemaTransformer bundle;
  sourcemeta::core::add(bundle,
                        sourcemeta::core::AlterSchemaMode::StaticAnalysis);
  return bundle.check(
      document, sourcemeta::core::schema_official_walker,
      sourcemeta::core::schema_official_resolver,
      [&traces](const auto &pointer, const auto &name, const auto &message,
                const auto &description) {
        traces.emplace_back(pointer, name, message, description);
      },
      default_dialect);
}

inline auto fix_for_readability(
    sourcemeta::core::JSON &document,
    const std::optional<std::string> &default_dialect = std::nullopt) -> bool {
  sourcemeta::core::SchemaTransformer bundle;
  sourcemeta::core::add(bundle, sourcemeta::core::AlterSchemaMode::Readability);
  return bundle.apply(
      document, sourcemeta::core::schema_official_walker,
      sourcemeta::core::schema_official_resolver,
      [](const auto &, const auto &name, const auto &, const auto &) {
        ADD_FAILURE() << name;
      },
      default_dialect);
}

inline auto fix_for_static_analysis(
    sourcemeta::core::JSON &document,
    const std::optional<std::string> &default_dialect = std::nullopt) -> bool {
  sourcemeta::core::SchemaTransformer bundle;
  sourcemeta::core::add(bundle,
                        sourcemeta::core::AlterSchemaMode::StaticAnalysis);
  return bundle.apply(
      document, sourcemeta::core::schema_official_walker,
      sourcemeta::core::schema_official_resolver,
      [](const auto &, const auto &name, const auto &, const auto &) {
        ADD_FAILURE() << name;
      },
      default_dialect);
}

#endif
