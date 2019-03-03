cmake_minimum_required (VERSION 3.2)

project (X4Converter)
include(GNUInstallDirs)
set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 11)
find_package( ZLIB REQUIRED )
find_package(Boost COMPONENTS filesystem REQUIRED)

#TODO refactor
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
	GIT_REPOSITORY https://github.com/assimp/assimp.git
	GIT_TAG v4.1.0
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/assimp -DCMAKE_CXX_FLAGS="-fPIC")
set(assimp_LIB ${CMAKE_BINARY_DIR}/assimp/lib/libassimp.so)
set(assimp_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/assimp/include ${CMAKE_BINARY_DIR}/external/src/assimp/code)

add_subdirectory(X4ConverterTools)
add_subdirectory(X4ConverterApp)
