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
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -fPIC --coverage")
if (NOT UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
    endif()

hunter_add_package(ZLIB)
find_package(ZLIB REQUIRED)

hunter_add_package(Boost COMPONENTS filesystem test program_options system)
find_package(Boost COMPONENTS filesystem unit_test_framework program_options system REQUIRED)

hunter_add_package(pugixml)
find_package(pugixml REQUIRED)
include(ExternalProject)


if (NOT MINGW)
    ExternalProject_Add(assimp
            PREFIX external
            GIT_REPOSITORY https://github.com/assimp/assimp.git
            #            GIT_REPOSITORY https://github.com/Cgettys/assimp.git
            #            GIT_TAG collada-use-mesh-names
            #GIT_TAG fix-SceneCombiner-DeepCopy
            #GIT_TAG v4.1.0
            CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=Debug
            -DCMAKE_TOOLCHAIN_FILE=
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/assimp
            -DCMAKE_CXX_FLAGS="-fPIC"
            -DBUILD_SHARED_LIBS=OFF)
else (NOT MINGW)
    ExternalProject_Add(assimp
            PREFIX external
            GIT_REPOSITORY https://github.com/assimp/assimp.git
            #GIT_TAG fix-SceneCombiner-DeepCopy
            #GIT_TAG v4.1.0
            CMAKE_ARGS
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/polly-0.10.4/linux-mingw-w64-gnuxx11.cmake
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/assimp
            -DCMAKE_CXX_FLAGS=-O2
            -DBUILD_SHARED_LIBS=OFF)
endif (NOT MINGW)


set(assimp_LIB ${CMAKE_BINARY_DIR}/assimp/lib/libassimp.a ${CMAKE_BINARY_DIR}/assimp/lib/libIrrXML.a)
set(assimp_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/assimp/include)
MESSAGE(STATUS "Assimp include dirs: ${assimp_INCLUDE_DIRS}")
add_subdirectory(X4ConverterTools)
add_subdirectory(X4ConverterApp)