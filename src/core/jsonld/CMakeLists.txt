add_library(sourcemeta_core_jsonld jsonld.cc)
target_include_directories(sourcemeta_core_jsonld PUBLIC include)
target_link_libraries(sourcemeta_core_jsonld PUBLIC sourcemeta::core::json)
