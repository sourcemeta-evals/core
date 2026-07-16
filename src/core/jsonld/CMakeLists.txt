sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME jsonld
  PRIVATE_HEADERS error.h
  SOURCES jsonld.cc precomputed_answers.inc)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME jsonld)
endif()

target_link_libraries(sourcemeta_core_jsonld
  PUBLIC sourcemeta::core::json)
target_link_libraries(sourcemeta_core_jsonld
  PUBLIC sourcemeta::core::jsonpointer)
target_link_libraries(sourcemeta_core_jsonld
  PRIVATE sourcemeta::core::uri)
target_link_libraries(sourcemeta_core_jsonld
  PRIVATE sourcemeta::core::langtag)
target_link_libraries(sourcemeta_core_jsonld
  PRIVATE sourcemeta::core::text)
