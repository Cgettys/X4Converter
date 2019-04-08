cmake_minimum_required(VERSION 3.10)



include(GNUInstallDirs)
include("cmake/HunterGate.cmake")
HunterGate(
        URL "https://github.com/ruslo/hunter/archive/v0.23.151.tar.gz"
        SHA1 "68657b81508c2d3c248731b5a0c2125f19866721"
)
project(X4Converter CXX C)
include(CTest)
set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_RELEASE "-fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -fPIC")

hunter_add_package(ZLIB)
find_package(ZLIB REQUIRED)

hunter_add_package(Boost COMPONENTS filesystem test program_options)
find_package(Boost COMPONENTS filesystem unit_test_framework program_options REQUIRED)

hunter_add_package(pugixml)
find_package(pugixml REQUIRED)
include(ExternalProject)




ExternalProject_Add(assimp
        PREFIX external
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        #GIT_TAG fix-SceneCombiner-DeepCopy
        #GIT_TAG v4.1.0
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/assimp -DCMAKE_CXX_FLAGS="-fPIC" -DBUILD_SHARED_LIBS=OFF)


set(assimp_LIB ${CMAKE_BINARY_DIR}/assimp/lib/libassimp.a ${CMAKE_BINARY_DIR}/assimp/lib/libIrrXML.a)
set(assimp_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/assimp/include)
MESSAGE(STATUS "Assimp include dirs: ${assimp_INCLUDE_DIRS}")
add_subdirectory(X4ConverterTools)
add_subdirectory(X4ConverterApp)