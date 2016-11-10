
#include "Consumer.h"
#include "FunctionTestDataFile.h"
#include "JsonReader.h"
#include "JsonWriter.h"
#include "optionparser.h"
#include "Results.h"
#include "utils.h"
#include "Results.h"
#include "writer.h"
#include "UnitTestData.h"
#include "TestData.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <exception>

#include <clang/Parse/ParseAST.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/HeaderSearch.h>
#include <llvm/Support/Host.h>


#include <sys/resource.h>

static const char *mocksFileNameSuffix = "-mocks.json";
static const char *funcsFileNameSuffix = "-unitTest.json";



// static TemplateInfo outputFilesSuffix[] = { 
//    { std::string(std::getenv("TEMPLATE_DIR")) + std::string("/mock.template"), "-mocks.h" },
//    { std::string(std::getenv("TEMPLATE_DIR")) + std::string("/UT.template"), "-ugtest.cpp" },
//    { std::string(std::getenv("TEMPLATE_DIR")) + std::string("/serialization.template"), "-serialization.h" },
//    { std::string(std::getenv("TEMPLATE_DIR")) + std::string("/serialization-struct.template"), "-serialization-struct.h"}
// };

static const char* templateFilesName[] = { 
   "-mocks.h",
   "-ugtest.cpp",
   "-serialization.h",
   "-serialization-struct.h"
};


int main(int argc, const char *argv[])
{
   try {
      OptionParser::get().createOptionMap(argc, argv);
      
      if( OptionParser::get().isHelp() ){
         OptionParser::get().printHelp();
         return EXIT_SUCCESS;
      }
      
      if( OptionParser::get().isVersion() ){
         OptionParser::get().printVersion();
         return EXIT_SUCCESS;
      }
      
      FunctionTestDataFile::get().computeAST();
      
      // test UnitTestData
      UnitTestData unitTestData( FunctionsToUnitTest::get().declKeySetMap, FunctionsToMock::get().declKeySetMap);
      unitTestData.buildCollectionTree();

//       TestData testData(FunctionsToUnitTest::get().declKeySetMap, FunctionsToMock::get().declKeySetMap );
//       testData.buildTreeFromAST();
      
      if( OptionParser::get().isExampleEnabled() ) {
         JsonWriter jsonWriter( FunctionTestDataFile::get(), unitTestData );
         // do we have to check if OptionParser::get().getOutputName() is empty ?
         jsonWriter.templateFile(OptionParser::get().getOutputName());
         return EXIT_SUCCESS;
      }

      JsonReader reader(unitTestData);
      reader.parse( FunctionTestDataFile::get(), OptionParser::get().getJsonFileName() );
      
      std::string outputFileName = FunctionTestDataFile::get().getProjectDescription().getOutputFileName();
//       Writer writer( unitTestData, outputFileName, FunctionTestDataFile::get().getCompilerInstance().getSourceManager());
//       writer.createFiles();
      
      FileWriterManager filesWriterMgr(outputFileName, unitTestData, FunctionTestDataFile::get().getCompilerInstance().getSourceManager());
      filesWriterMgr.add( new MockWriter() );
      filesWriterMgr.add( new UnitTestFileWriter() );
      filesWriterMgr.add( new SerializationWriter() );
      filesWriterMgr.add( new StructuresToSerializeWriter() );
      filesWriterMgr.write();
      

   } catch (std::exception &e){
      std::cout << e.what() << std::endl;
      OptionParser::get().printAll();
      return EXIT_FAILURE;
   }
   //check if temporary files are still there, close them and rm them
   //rm the fakesource.c 
   return EXIT_SUCCESS;   
}

