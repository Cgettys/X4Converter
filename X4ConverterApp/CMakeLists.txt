 
add_executable(X4ConverterApp src/main.cpp)
target_link_libraries(X4ConverterApp X4ConverterTools Boost::system Boost::filesystem Boost::program_options ${assimp_LIB} pugixml ${ZLIB_LIB})
target_include_directories(X4ConverterApp PRIVATE ${assimp_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
