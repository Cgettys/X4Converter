cmake_minimum_required(VERSION 3.13)
project(X4Converter CXX C)
include(CTest)
include(GNUInstallDirs)
set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")
find_package(ZLIB REQUIRED)
find_package(Boost COMPONENTS filesystem unit_test_framework program_options REQUIRED)

include(ExternalProject)
ExternalProject_Add(pugixml
        PREFIX external
        GIT_REPOSITORY https://github.com/zeux/pugixml.git
        GIT_TAG v1.9
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/pugixml -DCMAKE_CXX_FLAGS="-fPIC")

set(pugixml_LIB ${CMAKE_BINARY_DIR}/pugixml/lib/libpugixml.a)
set(pugixml_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/pugixml/include)
include(ExternalProject)
ExternalProject_Add(assimp
        PREFIX external
        GIT_REPOSITORY https://github.com/cgettys/assimp.git
        #GIT_TAG fix-SceneCombiner-DeepCopy
        #GIT_TAG v4.1.0
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/assimp -DCMAKE_CXX_FLAGS="-fPIC")
set(assimp_LIB ${CMAKE_BINARY_DIR}/assimp/lib/libassimp.so)
set(assimp_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/assimp/include)
MESSAGE(STATUS "Assimp include dirs: ${assimp_INCLUDE_DIRS}")
add_subdirectory(X4ConverterTools)
add_subdirectory(X4ConverterApp)