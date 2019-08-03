


set("LIB_SRCS"
        src/Conversion.cpp
        src/model/Material.cpp
        src/model/MaterialCollection.cpp
        src/model/MaterialLibrary.cpp
        src/ani/AnimFile.cpp
        src/ani/Header.cpp
        src/ani/AnimDesc.cpp
        src/ani/Keyframe.cpp
        src/util/AssimpUtil.cpp
        src/util/DXUtil.cpp
        src/xmf/XmfDataBuffer.cpp
        src/xmf/XmfDataBufferDesc.cpp
        src/xmf/XmfMaterial.cpp
        src/xmf/XmfVertexElement.cpp
        src/xmf/XmfFile.cpp
        src/xmf/XmfHeader.cpp src/ConversionContext.cpp src/model/Component.cpp include/X4ConverterTools/model/Component.h src/model/Connection.cpp include/X4ConverterTools/model/Connection.h src/model/Part.cpp include/X4ConverterTools/model/Part.h include/X4ConverterTools/model/AbstractElement.h src/model/VisualLod.cpp include/X4ConverterTools/model/VisualLod.h src/model/CollisionLod.cpp include/X4ConverterTools/util/FormatUtil.h src/model/AbstractElement.cpp src/model/Light.cpp include/X4ConverterTools/model/Light.h src/model/Layer.cpp include/X4ConverterTools/model/Layer.h)
add_library(X4ConverterTools STATIC ${LIB_SRCS})
target_compile_definitions(X4ConverterTools PRIVATE "X4ConverterTools_TEST=TRUE") # TODO fixme
target_link_libraries(X4ConverterTools PUBLIC Boost::filesystem Boost::unit_test_framework pugixml ${ZLIB_LIBRARIES} ${assimp_LIB})
# TODO fix so that proper includes are passed onto dependent projects?
target_include_directories(X4ConverterTools PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src
        Vendor/half
        ${ZLIB_INCLUDE_DIRS}
        ${assimp_INCLUDE_DIRS})
# Manual dependency for external project
add_dependencies(X4ConverterTools pugixml assimp)

# 'make install' to the correct locations (provided by GNUInstallDirs).
#install(TARGETS X4ConverterTools EXPORT X4ConverterToolsConfig
#        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows
#install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
#install(EXPORT X4ConverterToolsConfig DESTINATION share/X4ConverterTools/cmake)

# This makes the project importable from the build directory
export(TARGETS X4ConverterTools FILE X4ConverterToolsConfig.cmake)

if (NOT (WIN32 OR MINGW))
    add_subdirectory(test)
endif ()