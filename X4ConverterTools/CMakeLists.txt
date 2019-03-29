


set("LIB_SRCS"
        src/API.cpp
        src/Component.cpp
        src/ComponentPart.cpp
        src/ComponentPartLod.cpp
        src/Material.cpp
        src/MaterialCollection.cpp
        src/MaterialLibrary.cpp
        src/ani/AnimFile.cpp
        src/ani/Header.cpp
        src/ani/AnimDesc.cpp
        src/ani/Keyframe.cpp
        src/util/AssimpUtil.cpp
        src/util/DXUtil.cpp
        src/util/PathUtil.cpp
        src/xmf/XmfDataBuffer.cpp
        src/xmf/XmfDataBufferDesc.cpp
        src/xmf/XmfExporter.cpp
        src/xmf/XmfImporter.cpp
        src/xmf/XmfMaterial.cpp
        src/xmf/XmfVertexElement.cpp
        src/xmf/XuMeshFile.cpp
        src/xmf/XmfHeader.cpp)
add_library(X4ConverterTools SHARED ${LIB_SRCS})
target_link_libraries(X4ConverterTools PUBLIC Boost::filesystem ${pugixml_LIB} ${ZLIB_LIBRARIES} ${assimp_LIB})
# TODO fix so that proper includes are passed onto dependent projects?
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