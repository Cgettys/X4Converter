 
add_executable(X4Converter src/main.cpp)
target_link_libraries(X4Converter ${X4ConverterLib_LIBRARIES} Boost::filesystem)
target_include_directories(X4Converter PRIVATE ${Boost_INCLUDE_DIRS})