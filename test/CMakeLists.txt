file(GLOB sources *.c *.cc *.cpp *.h *.hh)

add_executable(kyaml_test ${sources})
target_link_libraries(kyaml_test kyaml ${GMOCK_LIBRARIES} GTest::GTest GTest::Main)

gtest_discover_tests(kyaml_test)
