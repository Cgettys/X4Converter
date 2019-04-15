
enable_testing()


# creates the executable
add_executable(test_executable testUtil.cpp testUtil.h test.cpp model/testPart.cpp xmf/testXuMeshFile.cpp ani/testAnimFile.cpp xmf/testXmfMaterial.cpp xmf/testXmfHeader.cpp xmf/testXmfDataBufferDesc.cpp model/testLod.cpp model/testComponent.cpp model/testConnection.cpp)
# indicates the include paths
target_include_directories(test_executable PRIVATE
        ${assimp_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${pugixml_INCLUDE_DIRS}
        ${ZLIB_INCLUDE_DIRS})
# indicates the shared library variant
target_compile_definitions(test_executable PRIVATE "X4ConverterTools_TEST=TRUE")
#set_target_properties(test_executable PROPERTIES COMPILE_FLAGS "--coverage")
# indicates the link paths
target_link_libraries(test_executable Boost::unit_test_framework ${assimp_LIB} ${ZLIB_LIB} pugixml X4ConverterTools gcov)
# declares a test with our executable
add_test(NAME tests COMMAND test_executable)