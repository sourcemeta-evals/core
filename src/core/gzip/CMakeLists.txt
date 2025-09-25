sourcemeta_library(NAMESPACE sourcemeta PROJECT core NAME gzip SOURCES gzip.cc)

if(SOURCEMETA_CORE_INSTALL)
  sourcemeta_library_install(NAMESPACE sourcemeta PROJECT core NAME gzip)
endif()

# Unrelated change
target_link_libraries(sourcemeta_core_gzip PRIVATE ZLIB::ZLIB)
