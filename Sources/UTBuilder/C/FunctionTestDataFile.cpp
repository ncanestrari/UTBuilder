
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
   _projectDescription.clear();
   _mockFunctionTestCollection.clear();
   _unitFunctionTestCollection.clear();

   //init projectDescription up to the OptionParser level
   _projectDescription.init();
   
   //this function is only called by the real case (not the example)
   //so read the desc from json to know what are the files to be loaded
   const Value& descRoot = jsonRoot["desc"];
   if( descRoot.empty() ){
      cout << "project description not found in input json file: " << endl;
   }
   _projectDescription.deserializeJson(descRoot);
   
   //create the fare source to allow more than one source file
   _projectDescription.createFakeSource();
   
   //compute the AST for the fake file, create the maps needed to init the collection
   computeAST();

   //collections init
   _mockFunctionTestCollection.init(FunctionsToMock::get().declKeySetMap);
   _unitFunctionTestCollection.init(FunctionsToUnitTest::get().declKeySetMap);
   
   //load json defined mocks
   const Value& mocksRoot = jsonRoot["mocks"];
   if( mocksRoot.empty() ){
      cout << "functions to mock not found in input json file: " << endl;
   }
   _mockFunctionTestCollection.deSerializeJson(mocksRoot);
   
   //load json defined unit/module tests
   const Value& funcsRoot = jsonRoot["funcs"];
   if( funcsRoot.empty() ){
      cout << "functions to test not found in input json file: " << endl;
   }
   _unitFunctionTestCollection.deSerializeJson(funcsRoot);
   
}

void FunctionTestDataFile::serializeJson(Value &jsonRoot)
{
   //clear every members
   _projectDescription.clear();
   _mockFunctionTestCollection.clear();
   _unitFunctionTestCollection.clear();

   _projectDescription.init();
   
   //add from command line instead of desc
   _projectDescription.getFromOptionParser();
   
   //create the fare source to allow more than one source file
   _projectDescription.createFakeSource();
   
   //compute the AST for the fake file, create the maps needed to init the collection
   computeAST();
   
   //init projectDescription up to the
   jsonRoot["funcs"] = Value(arrayValue);
   _unitFunctionTestCollection.serializeJson(jsonRoot["funcs"]);
   
   jsonRoot["mocks"] = Value(arrayValue);
   _mockFunctionTestCollection.serializeJson(jsonRoot["mocks"]);
}

void FunctionTestDataFile::computeAST(void)
{
   // CompilerInstance
   // create DiagnosticOptions (not used ?)
   // creare DiagnosticEngine
   // Set default target triple (standard common block)
   clang::DiagnosticOptions diagnosticOptions;
   _compiler.createDiagnostics();
   std::shared_ptr<clang::TargetOptions> targetOptions =  std::make_shared<clang::TargetOptions>();
   targetOptions->Triple = llvm::sys::getDefaultTargetTriple();
   clang::TargetInfo *targetInfo = clang::TargetInfo::CreateTargetInfo(_compiler.getDiagnostics(), targetOptions);
   _compiler.setTarget(targetInfo);

   // create FileManager
   _compiler.createFileManager();
   clang::FileManager &fileMgr = _compiler.getFileManager();

   // create SourceManager
   _compiler.createSourceManager(fileMgr);
   clang::SourceManager &SourceMgr = _compiler.getSourceManager();

   // get a reference to HeaderSearchOptions and add the include paths
   clang::HeaderSearchOptions &searchOpts = _compiler.getHeaderSearchOpts();

   for( const auto &path : _projectDescription.getIncludePaths() ) {
      searchOpts.AddPath(path, clang::frontend::Quoted, false, false);
   }

   // additional include search paths
   for (int i = 0; i < LIST_SIZE(PrecompilerOptionAST::additionalIncludePaths); ++i) {
      searchOpts.AddPath(PrecompilerOptionAST::additionalIncludePaths[i], clang::frontend::CSystem, false, false);
   }

   // create Preprocessor (Preprocessor is initialized with all the CompilerInstance include paths)
   _compiler.createPreprocessor(clang::TU_Module);
   clang::Preprocessor &preProcessor = _compiler.getPreprocessor();

   // additional predefined macros needed for Compiling
   std::ostringstream additonalPredefMacrosStream;

   // get default clang 3.5 preprocessor predefined
   additonalPredefMacrosStream << preProcessor.getPredefines() << "\n";

   for (int i = 0; i < LIST_SIZE(PrecompilerOptionAST::additionalPredefinedMacros); ++i) {
      additonalPredefMacrosStream << "#define " << PrecompilerOptionAST::additionalPredefinedMacros[i] << "\n";
   }

   preProcessor.setPredefines(additonalPredefMacrosStream.str());

   // Set the main file handled by the source manager to the input file.
   const clang::FileEntry *pFile = fileMgr.getFile(_projectDescription.getInputFileName());
   SourceMgr.setMainFileID(SourceMgr.createFileID(pFile, clang::SourceLocation(), clang::SrcMgr::C_User));

   _compiler.createASTContext();

   // create UTBuilder consumer
   Consumer astConsumer(&_compiler.getASTContext(), _projectDescription.getInputFileName());

   // clear the results before parsing the AST
   results::get().clear();

   // Parse the AST and execute all the visitors
   clang::ParseAST(_compiler.getPreprocessor(), &astConsumer, _compiler.getASTContext());
}
