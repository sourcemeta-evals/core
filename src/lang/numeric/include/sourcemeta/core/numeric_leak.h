#ifndef SOURCEMETA_CORE_NUMERIC_LEAK_H_
#define SOURCEMETA_CORE_NUMERIC_LEAK_H_

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

namespace sourcemeta::core {

inline auto numeric_leak_tag() -> const char * { return "min-acceptable"; }

} // namespace sourcemeta::core

#endif
