

#include "ClangCompiler.h"
#include "JsonReader.h"
#include "JsonWriter.h"
#include "optionparser.h"

#include "Writers.h"
#include "WritersManager.h"
#include "UnitTestData.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <exception>



int main(int argc, const char *argv[])
{

   OptionParser optionParser;
   
   try {
      
      optionParser.createOptionMap(argc, argv);
      
      if( optionParser.isHelp() ){
         optionParser.printHelp();
         return EXIT_SUCCESS;
      }
      
      if( optionParser.isVersion() ){
         optionParser.printVersion();
         return EXIT_SUCCESS;
      }
      
      
      ClangCompiler clangCompiler;
      clangCompiler.computeAST( optionParser );
      
      
      
      // create data from AST
      UnitTestData unitTestData( &clangCompiler );
      
      
      if( optionParser.isExampleEnabled() ) {
         //write json example file to be filled
         JsonWriter jsonWriter( unitTestData );
         // do we have to check if OptionParser::get().getOutputName() is empty ?
         jsonWriter.templateFile( optionParser.getOutputName() );
      }
      else {

         //read the json input file and write unit test files
         JsonReader reader(unitTestData);
         reader.parse( optionParser.getJsonFileName() );

         std::string outputFileName = clangCompiler.getProjectDescription().getOutputFileName();
 
         WritersManager writerManager(outputFileName, unitTestData, clangCompiler );
 
         //create the writers and add to the manager  
         writerManager.add( new MockWriter() );
         writerManager.add( new UnitTestFileWriter() );
         writerManager.add( new SerializationWriter() );
         writerManager.add( new StructuresToSerializeWriter() );
         writerManager.add( new CMakeFileWriter() );
         writerManager.add( new GlobalsWriter() );

         writerManager.write();
      }

   } 
   catch (std::exception &e) {
      
      std::cout << e.what() << std::endl;
      optionParser.printAll();
      return EXIT_FAILURE;
   }
   catch ( ... ) {
      
      std::cout << "unhandled exception ..." << std::endl;
      return EXIT_FAILURE;
   }
   
   return EXIT_SUCCESS;   
   
}

