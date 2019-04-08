 
add_executable(X4ConverterApp src/main.cpp)
target_link_libraries(X4ConverterApp X4ConverterTools Boost::filesystem Boost::program_options ${assimp_LIB} pugixml ${ZLIB_LIB})
target_include_directories(X4ConverterApp PRIVATE ${Boost_INCLUDE_DIRS} ${assimp_INCLUDE_DIRS} ${pugixml_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
