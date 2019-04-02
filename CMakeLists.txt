cmake_minimum_required(VERSION 3.13)
project(X4Converter CXX C)
include(CTest)
include(GNUInstallDirs)
set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")

#find_package(ZLIB REQUIRED)

#set(Boost_DEBUG ON)
if (WIN32 OR MINGW)
    #set(Boost_USE_STATIC_RUNTIME ON)
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_ARCHITECTURE -x64)
    set(Boost_NO_BOOST_CMAKE ON)
    set(Boost_THREADAPI "win32")
    set(BOOST_ROOT /home/cg/boost-mingw)
    set(BOOST_COMPILER -mgw73)
    set(BOOST_INCLUDEDIR /home/cg/boost-mingw/include)
    set(BOOST_LIBRARYDIR /home/cg/boost-mingw/lib/)
    set(Boost_NO_SYSTEM_PATHS ON)
    link_libraries(-static-libgcc -static-libstdc++)
endif ()
find_package(Boost COMPONENTS filesystem unit_test_framework program_options REQUIRED)

include(ExternalProject)


ExternalProject_Add(zlib
        PREFIX external
        GIT_REPOSITORY       https://github.com/madler/zlib.git
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/zlib -DCMAKE_CXX_FLAGS="-fPIC")


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