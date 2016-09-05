cmake_minimum_required(VERSION 3.0.2)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
message(STATUS "output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

SET(CMAKE_CXX_COMPILER "g++-4.8")
SET(CMAKE_C_COMPILER "gcc-4.8") 
SET(CMAKE_CXX_COMPILER_VERSION "4.8")
SET(CMAKE_C_COMPILER_VERSION "4.8")
SET(CMAKE_CXX_FLAGS "-fmax-errors=10")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")


message(STATUS "C   compiler: ${CMAKE_C_COMPILER_ID}  version: ${CMAKE_C_COMPILER_VERSION}" )
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID} version: ${CMAKE_CXX_COMPILER_VERSION}" )

# generates a json file containing the compiler calls for all translation units
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

    
set ( SOURCE_FILES
   C/context.cpp
   C/template.cpp
   )

set ( HEADER_FILES
   Include/plustache_types.hpp
   Include/context.hpp
   Include/template.hpp
   )

SET( CMAKE_INCLUDE_CURRENT_DIR TRUE )
include_directories( $(CMAKE_CURRENT_SOURCE_DIR)/Include )


# add boost includes
include_directories( /.${Boost_INCLUDE_DIRS})

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES )


add_library( Plustache STATIC ${SOURCE_FILES} )


