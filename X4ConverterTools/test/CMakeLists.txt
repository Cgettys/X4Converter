
enable_testing()


# creates the executable
add_executable(test_executable test.cpp)
# indicates the include paths
target_include_directories(test_executable PRIVATE ${Boost_INCLUDE_DIRS})
# indicates the shared library variant
target_compile_definitions(test_executable PRIVATE "BOOST_TEST_DYN_LINK=1")
# indicates the link paths
target_link_libraries(test_executable ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
# declares a test with our executable
add_test(NAME test_xml COMMAND test_executable)