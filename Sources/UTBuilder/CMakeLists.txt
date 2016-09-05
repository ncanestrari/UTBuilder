
SET(CMAKE_CXX_COMPILER "g++-4.8")
SET(CMAKE_C_COMPILER "gcc-4.8") 
SET(CMAKE_CXX_COMPILER_VERSION "4.8")
SET(CMAKE_C_COMPILER_VERSION "4.8")
SET(CMAKE_CXX_FLAGS "-fmax-errors=10")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# generates a json file containing the compiler calls for all translation units
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )



set ( HEADER_FILES
   Include/MockVisitor.h
   Include/FuncUTDefVisitor.h
   Include/StructVisitor.h
   Include/TypedefVisitor.h
   Include/writer.h
   Include/Consumer.h
   Include/Action.h
   Include/utils.h
   )

set ( SOURCE_FILES
   C/main.cpp
   C/MockVisitor.cpp
   C/FuncUTDefVisitor.cpp
   C/StructVisitor.cpp
   C/TypedefVisitor.cpp
   C/writer.cpp
   C/Consumer.cpp
   C/Action.cpp
   C/utils.cpp
   )

SET( CMAKE_INCLUDE_CURRENT_DIR TRUE )

include_directories(${UTBuilder_SOURCE_DIR}/Sources/Plustache/Include)
include_directories(${UTBuilder_SOURCE_DIR}/Sources/UTBuilder/Include)

# LLVM package
find_package ( LLVM REQUIRED CONFIG)

llvm_map_components_to_libnames(LLVM_LIB_LIBRARY  core support option irreader )

# add LLVM includes
include_directories( /.${LLVM_INCLUDE_DIRS} )
add_definitions(${LLVM_DEFINITIONS})

find_package(Clang REQUIRED  )
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${Clang_DEFINITIONS})

find_package(Boost REQUIRED 
    COMPONENTS 
    system
    filesystem
    program_options
    thread
    regex
)

# add boost includes
include_directories( /.${Boost_INCLUDE_DIRS})

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES )


add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )

# Find the libraries that correspond to the LLVM components
# that we wish to use
#llvm_map_components_to_libnames(llvm_libs support core irreader)

link_directories("/./usr/lib64")

# statically link with LibClang
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${LLVM_LIB_LIBRARY} ${CLANG_LIBS} Plustache)

