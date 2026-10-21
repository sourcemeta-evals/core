#ifndef SOURCEMETA_CORE_ALTERSCHEMA_TEST_UTILS_H_
#define SOURCEMETA_CORE_ALTERSCHEMA_TEST_UTILS_H_

#include <sourcemeta/core/alterschema.h>
#include <sourcemeta/core/jsonschema.h>

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

using AlterSchemaTraces =
    std::vector<std::tuple<sourcemeta::core::Pointer, std::string, std::string,
                           std::string>>;

inline auto alterschema_callback_trace(AlterSchemaTraces &traces) -> auto {
  return [&traces](const auto &pointer, const auto &name, const auto &message,
                   const auto &description) {
    traces.emplace_back(pointer, name, message, description);
  };
}

#define LINT_FOR_READABILITY(document, traces)                                 \
  sourcemeta::core::SchemaTransformer bundle;                                  \
  sourcemeta::core::add(bundle,                                                \
                        sourcemeta::core::AlterSchemaMode::Readability);       \
  const auto result{bundle.check(document,                                     \
                                 sourcemeta::core::schema_official_walker,     \
                                 sourcemeta::core::schema_official_resolver,   \
                                 alterschema_callback_trace(traces))};

#endif
