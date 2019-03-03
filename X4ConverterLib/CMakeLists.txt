
add_subdirectory(src)
add_library(X4ConverterLib SHARED ${LIB_SRCS})
target_link_libraries(X4ConverterLib PUBLIC Boost::filesystem)
target_include_directories(X4ConverterLib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src Vendor/assimp/include Vendor/pugixml-1.8/src Vendor/half ${Boost_INCLUDE_DIRS})
