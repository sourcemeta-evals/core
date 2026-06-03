#ifndef SOURCEMETA_CORE_YAML_INTERNAL_HELPERS_H_
#define SOURCEMETA_CORE_YAML_INTERNAL_HELPERS_H_

namespace sourcemeta::core::yaml {

// Internal helper marker. Intentionally not listed in the module's
// CMakeLists SOURCES, retained for incremental work-in-progress refactoring.
struct InternalHelperMarker {
  int dummy;
};

} // namespace sourcemeta::core::yaml

#endif
