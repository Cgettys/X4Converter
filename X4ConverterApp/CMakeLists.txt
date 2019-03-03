 
add_executable(X4ConverterApp src/main.cpp)
target_link_libraries(X4ConverterApp X4ConverterTools Boost::filesystem)
target_include_directories(X4ConverterApp PRIVATE ${Boost_INCLUDE_DIRS})