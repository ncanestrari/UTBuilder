
#include "Consumer.h"
#include "FunctionTestDataFile.h"
#include "JsonReader.h"
#include "JsonWriter.h"
#include "optionparser.h"
#include "Results.h"
#include "utils.h"
#include "Results.h"
#include "writer.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include <clang/Parse/ParseAST.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/HeaderSearch.h>
#include <llvm/Support/Host.h>

static const char *mocksFileNameSuffix = "-mocks.json";
static const char *funcsFileNameSuffix = "-unitTest.json";


int main(int argc, const char *argv[])
{
   bool needToContinue = OptionParser::get().createOptionMap(argc, argv);
   if( !needToContinue ){
      return EXIT_SUCCESS;
   }
   
   if( OptionParser::get().isExampleEnabled() ) {
      JsonWriter jsonWriter( FunctionTestDataFile::get() );
      jsonWriter.templateFile(OptionParser::get().getOutputName());
      return EXIT_SUCCESS;
   }

   JsonReader reader;
   reader.parse( FunctionTestDataFile::get(), OptionParser::get().getJsonFileName() );


   Writer writer(FunctionTestDataFile::get().getProjectDescription().getOutputFileName(), FunctionTestDataFile::get().getCompilerInstance().getSourceManager());
   writer.createFiles();

   //rm the fakesource.c 
   
   return EXIT_SUCCESS;
}

