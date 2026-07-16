add_executable(sourcemeta_core_jsonld_test jsonld_test.cc)
target_link_libraries(sourcemeta_core_jsonld_test
  PRIVATE sourcemeta::core::jsonld
          sourcemeta::core::json
          sourcemeta::core::jsonpointer
          GTest::gtest
          GTest::gmock
          GTest::gtest_main)
add_test(NAME core.jsonld_test COMMAND sourcemeta_core_jsonld_test --gtest_brief=1)

add_executable(sourcemeta_core_jsonld_suite jsonld_suite.cc)
target_compile_definitions(sourcemeta_core_jsonld_suite
  PRIVATE JSONLD_SUITE_PATH="${PROJECT_SOURCE_DIR}/vendor/w3c-json-ld/tests")
target_link_libraries(sourcemeta_core_jsonld_suite
  PRIVATE sourcemeta::core::jsonld
          sourcemeta::core::json
          sourcemeta::core::text
          GTest::gtest
          GTest::gmock
          GTest::gtest_main)
add_test(NAME core.jsonld_suite COMMAND sourcemeta_core_jsonld_suite --gtest_brief=1)
