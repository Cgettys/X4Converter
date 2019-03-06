
add_subdirectory(src)
#https://rix0r.nl/blog/2015/08/13/cmake-guide/
add_library(X4ConverterTools SHARED ${LIB_SRCS})
target_link_libraries(X4ConverterTools PUBLIC Boost::filesystem ${pugixml_LIB} ${ZLIB_LIBRARIES} ${assimp_LIB} )
target_include_directories(X4ConverterTools PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src 
    Vendor/half
    ${pugixml_INCLUDE_DIRS} 
    ${ZLIB_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${assimp_INCLUDE_DIRS})

# Manual dependency for external project
add_dependencies(X4ConverterTools pugixml assimp)

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS X4ConverterTools EXPORT X4ConverterToolsConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT X4ConverterToolsConfig DESTINATION share/X4ConverterTools/cmake)

# This makes the project importable from the build directory
export(TARGETS X4ConverterTools FILE X4ConverterToolsConfig.cmake)

add_subdirectory(test)