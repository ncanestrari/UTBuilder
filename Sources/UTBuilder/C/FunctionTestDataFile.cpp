
#include "FunctionTestDataFile.h"

#include <clang/Parse/ParseAST.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/HeaderSearch.h>
#include <json/json.h>
#include <llvm/Support/Host.h>
#include <string>
#include <vector>

#include "Consumer.h"
#include "optionparser.h"
#include "PrecompilerOptionAST.h"
#include "utils.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using Json::Value;
using Json::arrayValue;



void FunctionTestDataFile::deSerializeJson(const Value &jsonRoot)
{
   //clear every members
   projectDescription.clear();
   mockFunctionTestCollection.clear();
   unitFunctionTestCollection.clear();

   //init projectDescription up to the OptionParser level
   projectDescription.init();
   
   //this function is only called by the real case (not the example)
   //so read the desc from json to know what are the files to be loaded
   const Value& descRoot = jsonRoot["desc"];
   if( descRoot.empty() ){
      cout << "project description not found in input json file: " << endl;
   }
   projectDescription.deserializeJson(descRoot);
   
   //create the fare source to allow more than one source file
   projectDescription.createFakeSource();
   
   //compute the AST for the fake file, create the maps needed to init the collection
   computeAST();

   //collections init
   mockFunctionTestCollection.init(FunctionsToMock::get().declKeySetMap);
   unitFunctionTestCollection.init(FunctionsToUnitTest::get().declKeySetMap);
   
   //load json defined mocks
   const Value& mocksRoot = jsonRoot["mocks"];
   if( mocksRoot.empty() ){
      cout << "functions to mock not found in input json file: " << endl;
   }
   mockFunctionTestCollection.deSerializeJson(mocksRoot);
   
   //load json defined unit/module tests
   const Value& funcsRoot = jsonRoot["funcs"];
   if( funcsRoot.empty() ){
      cout << "functions to test not found in input json file: " << endl;
   }
   unitFunctionTestCollection.deSerializeJson(funcsRoot);
   
}

void FunctionTestDataFile::serializeJson(Value &jsonRoot)
{
   //clear every members
   projectDescription.clear();
   mockFunctionTestCollection.clear();
   unitFunctionTestCollection.clear();

   projectDescription.init();
   
   //add from command line instead of desc
   vector<string> filenames;
   OptionParser::get().getFileNames(filenames);
   projectDescription.setFileNames(filenames);
   projectDescription.setOutputFileName(OptionParser::get().getOutputName());
   
   //create the fare source to allow more than one source file
   projectDescription.createFakeSource();
   
   //compute the AST for the fake file, create the maps needed to init the collection
   computeAST();
   
   //init projectDescription up to the
   jsonRoot["funcs"] = Value(arrayValue);
   unitFunctionTestCollection.serializeJson(jsonRoot["funcs"]);
   
   jsonRoot["mocks"] = Value(arrayValue);
   mockFunctionTestCollection.serializeJson(jsonRoot["mocks"]);
}

void FunctionTestDataFile::computeAST(void)
{
   // CompilerInstance
   // create DiagnosticOptions (not used ?)
   // creare DiagnosticEngine
   // Set default target triple (standard common block)
   clang::DiagnosticOptions diagnosticOptions;
   compiler.createDiagnostics();
   std::shared_ptr<clang::TargetOptions> targetOptions =  std::make_shared<clang::TargetOptions>();
   targetOptions->Triple = llvm::sys::getDefaultTargetTriple();
   clang::TargetInfo *targetInfo = clang::TargetInfo::CreateTargetInfo(compiler.getDiagnostics(), targetOptions);
   compiler.setTarget(targetInfo);

   // create FileManager
   compiler.createFileManager();
   clang::FileManager &fileMgr = compiler.getFileManager();

   // create SourceManager
   compiler.createSourceManager(fileMgr);
   clang::SourceManager &SourceMgr = compiler.getSourceManager();

   // get a reference to HeaderSearchOptions and add the include paths
   clang::HeaderSearchOptions &searchOpts = compiler.getHeaderSearchOpts();

   for( const auto &path : projectDescription.getIncludePaths() ) {
      searchOpts.AddPath(path, clang::frontend::Quoted, false, false);
   }

   // additional include search paths
   for (int i = 0; i < LIST_SIZE(PrecompilerOptionAST::additionalIncludePaths); ++i) {
      searchOpts.AddPath(PrecompilerOptionAST::additionalIncludePaths[i], clang::frontend::CSystem, false, false);
   }


   // create Preprocessor (Preprocessor is initialized with all the CompilerInstance include paths)
   compiler.createPreprocessor(clang::TU_Module);
   clang::Preprocessor &preProcessor = compiler.getPreprocessor();

   // additional predefined macros needed for Compiling
   std::ostringstream additonalPredefMacrosStream;

   // get default clang 3.5 preprocessor predefined
   additonalPredefMacrosStream << preProcessor.getPredefines() << "\n";

   for (int i = 0; i < LIST_SIZE(PrecompilerOptionAST::additionalPredefinedMacros); ++i) {
      additonalPredefMacrosStream << "#define " << PrecompilerOptionAST::additionalPredefinedMacros[i] << "\n";
   }

   preProcessor.setPredefines(additonalPredefMacrosStream.str());

   // Set the main file handled by the source manager to the input file.
   const clang::FileEntry *pFile = fileMgr.getFile(projectDescription.getInputFileName());
   SourceMgr.setMainFileID(SourceMgr.createFileID(pFile, clang::SourceLocation(), clang::SrcMgr::C_User));

   compiler.createASTContext();

   // create UTBuilder consumer
   Consumer astConsumer(&compiler.getASTContext(), projectDescription.getInputFileName());

   // clear the results before parsing the AST
   results::get().clear();

   // Parse the AST and execute all the visitors
   clang::ParseAST(compiler.getPreprocessor(), &astConsumer, compiler.getASTContext());
}
