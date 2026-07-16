sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME jsonld
  SOURCES jsonld.cc)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME jsonld)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(sourcemeta_core_jsonld
  PUBLIC sourcemeta::core::json)
target_link_libraries(sourcemeta_core_jsonld
  PUBLIC sourcemeta::core::jsonpointer)
target_link_libraries(sourcemeta_core_jsonld
  PUBLIC sourcemeta::core::uri)
target_link_libraries(sourcemeta_core_jsonld
  PRIVATE sourcemeta::core::text)
