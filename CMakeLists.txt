cmake_minimum_required(VERSION 3.0.2)


project(UTBuilder)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake) 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_INSTALL_PREFIX /opt/UTBuilder)

add_subdirectory(Sources/Plustache)
add_subdirectory(Sources/JsonCpp)
add_subdirectory(Sources/UTBuilder)

