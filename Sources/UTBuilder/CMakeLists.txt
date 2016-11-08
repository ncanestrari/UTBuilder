
set(CMAKE_CXX_COMPILER "g++-4.8")
set(CMAKE_C_COMPILER "gcc-4.8") 
set(CMAKE_CXX_COMPILER_VERSION "4.8")
set(CMAKE_C_COMPILER_VERSION "4.8")
set(CMAKE_CXX_FLAGS "-fmax-errors=10")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set ( HEADER_FILES
   Include/Consumer.h
   Include/DataFile.h
   Include/FuncUTDefVisitor.h
   Include/FunctionTestCollection.h
   Include/FunctionTestContent.h
   Include/FunctionTestData.h
   Include/JsonReader.h
   Include/JsonWriter.h
   Include/MockVisitor.h
   Include/NameValueTypeNode.h
   Include/NameValueTypeVisitor.h
   Include/NameValueTypeActions.h
   Include/PrecompilerOptionAST.h
   Include/ProjectDescription.h
   Include/Results.h
   Include/Serializable.h
   Include/StructVisitor.h
   Include/TreeNode.h
   Include/TreeNodeVisitor.h
   Include/TreeNodeActions.h
   Include/TestData.h
   Include/TypedefVisitor.h
   Include/globber.h
   Include/optionparser.h
   Include/UnitTestData.h
   Include/UnitTestDataUtils.h
   Include/utils.h
   Include/writer.h
   Include/writers.h
   )

set ( SOURCE_FILES
   C/main.cpp
   C/Consumer.cpp
   C/FuncUTDefVisitor.cpp
   C/FunctionTestCollection.cpp
   C/FunctionTestContent.cpp
   C/FunctionTestData.cpp
   C/FunctionTestDataFile.cpp
   C/JsonReader.cpp
   C/JsonWriter.cpp
   C/MockVisitor.cpp
   C/NameValueTypeNode.cpp
   C/NameValueTypeVisitor.cpp
   C/NameValueTypeActions.cpp
   C/ProjectDescription.cpp
   C/Results.cpp
   C/StructVisitor.cpp
   C/TreeNode.cpp
   C/TreeNodeVisitor.cpp
   C/TreeNodeActions.cpp
   C/TestData.cpp
   C/TypedefVisitor.cpp
   C/globber.cpp
   C/optionparser.cpp
   C/UnitTestData.cpp
   C/UnitTestDataUtils.cpp
   C/utils.cpp 
   C/writer.cpp
   C/Writers.cpp
   )

set( CMAKE_INCLUDE_CURRENT_DIR TRUE )

include_directories( ${UTBuilder_SOURCE_DIR}/Sources/Plustache/Include )
include_directories( ${UTBuilder_SOURCE_DIR}/Sources/UTBuilder/Include )
include_directories( ${UTBuilder_SOURCE_DIR}/Sources/JsonCpp/Include )

# LLVM package
find_package ( LLVM REQUIRED CONFIG)
llvm_map_components_to_libnames(LLVM_LIB_LIBRARY  core support option irreader )

# add LLVM includes
include_directories( /.${LLVM_INCLUDE_DIRS} )
add_definitions(${LLVM_DEFINITIONS})

find_package(Clang REQUIRED)
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${Clang_DEFINITIONS})

find_package(Boost REQUIRED COMPONENTS 
    system
    filesystem
    program_options
    thread
    regex )

# add boost includes
include_directories( /.${Boost_INCLUDE_DIRS})

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES )


add_executable( UTBuilder ${SOURCE_FILES} )


# statically link with LibClang
link_directories("/./usr/lib64")
target_link_libraries(UTBuilder ${Boost_LIBRARIES} ${LLVM_LIB_LIBRARY} ${CLANG_LIBS} Plustache JsonCpp)

set( TEMPLATE_DIR 
   ${UTBuilder_SOURCE_DIR}/Template )

#create launcher
set( UTBUILDERTEMPLATE ${UTBuilder_SOURCE_DIR}/cmake/launcher.cmake-template )
set( UTBUILDERLAUNCHER ${PROJECT_BINARY_DIR}/UTBuilderLauncher )
configure_file( ${UTBUILDERTEMPLATE} ${UTBUILDERLAUNCHER} )


# install 
install( TARGETS UTBuilder RUNTIME DESTINATION bin )
install( DIRECTORY ${TEMPLATE_DIR} DESTINATION . 
   FILES_MATCHING PATTERN "*")
install( FILES ${UTBUILDERLAUNCHER} DESTINATION bin 
   PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
               GROUP_EXECUTE GROUP_READ)




