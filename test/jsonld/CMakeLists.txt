add_executable(jsonld_test test.cc)
target_link_libraries(jsonld_test PRIVATE sourcemeta_core_jsonld GTest::gtest GTest::gtest_main)
add_test(NAME jsonld_test COMMAND jsonld_test)
