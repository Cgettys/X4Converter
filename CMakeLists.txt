cmake_minimum_required (VERSION 2.6)

project (X4Converter)
set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 11)
find_package(Boost COMPONENTS filesystem REQUIRED)
add_subdirectory(X4ConverterLib)
add_subdirectory(X4Converter)
