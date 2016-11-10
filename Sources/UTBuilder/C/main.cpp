

#include "ClangCompiler.h"
#include "JsonReader.h"
#include "JsonWriter.h"
#include "optionparser.h"

#include "Writers.h"
#include "WritersManager.h"
#include "UnitTestData.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <exception>



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
      
      
      ClangCompiler clangCompiler;
      clangCompiler.computeAST();
      
      // test UnitTestData
      UnitTestData unitTestData( FunctionsToUnitTest::get().declKeySetMap, FunctionsToMock::get().declKeySetMap);
      unitTestData.buildCollectionTree();
      
      
      if( OptionParser::get().isExampleEnabled() ) {
	 
         JsonWriter jsonWriter( unitTestData );
         // do we have to check if OptionParser::get().getOutputName() is empty ?
         jsonWriter.templateFile(OptionParser::get().getOutputName());
      }
      else {

	 JsonReader reader(unitTestData);
	 reader.parse( OptionParser::get().getJsonFileName() );
	 
	 std::string outputFileName = clangCompiler.getProjectDescription().getOutputFileName();
	 
	 WritersManager writerManager(outputFileName, unitTestData, clangCompiler.getInstance().getSourceManager());
	 writerManager.add( new MockWriter() );
	 writerManager.add( new UnitTestFileWriter() );
	 writerManager.add( new SerializationWriter() );
	 writerManager.add( new StructuresToSerializeWriter() );
	 writerManager.write();
      }

   } 
   catch (std::exception &e) {
      
      std::cout << e.what() << std::endl;
      OptionParser::get().printAll();
      return EXIT_FAILURE;
   }
   
   //check if temporary files are still there, close them and rm them
   //rm the fakesource.c 
   return EXIT_SUCCESS;   
   
}

