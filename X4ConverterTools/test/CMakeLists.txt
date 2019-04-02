
enable_testing()


# creates the executable
add_executable(test_executable test.cpp xmf/testXuMeshFile.cpp ani/testAnimFile.cpp xmf/testXmfMaterial.cpp xmf/testXmfHeader.cpp xmf/testXmfDataBufferDesc.cpp)
# indicates the include paths
target_include_directories(test_executable PRIVATE
        ${assimp_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${pugixml_INCLUDE_DIRS}
        ${ZLIB_INCLUDE_DIRS})
# indicates the shared library variant
target_compile_definitions(test_executable PRIVATE "BOOST_TEST_DYN_LINK=1" "X4ConverterTools_TEST=TRUE")
# indicates the link paths
target_link_libraries(test_executable ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${assimp_LIB} ${ZLIB_LIB} X4ConverterTools)
# declares a test with our executable
add_test(NAME tests COMMAND test_executable)