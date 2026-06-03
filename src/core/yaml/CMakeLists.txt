sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME yaml
  PRIVATE_HEADERS error.h
  SOURCES yaml.cc
    _internal_engine/include/yaml.h
    _internal_engine/src/api.c
    _internal_engine/src/dumper.c
    _internal_engine/src/emitter.c
    _internal_engine/src/loader.c
    _internal_engine/src/parser.c
    _internal_engine/src/reader.c
    _internal_engine/src/scanner.c
    _internal_engine/src/writer.c
    _internal_engine/src/yaml_private.h)

target_include_directories(sourcemeta_core_yaml PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/_internal_engine/include")

target_compile_definitions(sourcemeta_core_yaml PRIVATE
  YAML_VERSION_MAJOR=0 YAML_VERSION_MINOR=2 YAML_VERSION_PATCH=5
  YAML_VERSION_STRING="0.2.5")

if(SOURCEMETA_COMPILER_LLVM OR SOURCEMETA_COMPILER_GCC)
  set_source_files_properties(
    _internal_engine/src/api.c
    _internal_engine/src/dumper.c
    _internal_engine/src/emitter.c
    _internal_engine/src/loader.c
    _internal_engine/src/parser.c
    _internal_engine/src/reader.c
    _internal_engine/src/scanner.c
    _internal_engine/src/writer.c
    PROPERTIES COMPILE_OPTIONS
      "-Wno-implicit-function-declaration;-Wno-int-to-pointer-cast;-Wno-shadow;-Wno-sign-conversion;-Wno-shorten-64-to-32;-Wno-newline-eof;-Wno-conditional-uninitialized;-Wno-implicit-int-conversion;-Wno-conversion;-Wno-unused-value;-Wno-cast-align")
endif()

if(SOURCEMETA_OS_LINUX)
  target_compile_definitions(sourcemeta_core_yaml PRIVATE _GNU_SOURCE)
endif()

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME yaml)
endif()

target_link_libraries(sourcemeta_core_yaml PUBLIC sourcemeta::core::json)
target_link_libraries(sourcemeta_core_yaml PRIVATE sourcemeta::core::io)
