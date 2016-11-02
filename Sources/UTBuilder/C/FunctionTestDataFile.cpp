
#include "FunctionTestDataFile.h"

#include <boost/filesystem.hpp>
using boost::filesystem::current_path;
using boost::filesystem::path;

#include <clang/Basic/TargetInfo.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Lex/HeaderSearch.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Parse/ParseAST.h>
using clang::DiagnosticOptions;
using clang::FileEntry;
using clang::FileManager;
using clang::frontend::CSystem;
using clang::frontend::Quoted;
using clang::HeaderSearchOptions;
using clang::ParseAST;
using clang::Preprocessor;
using clang::SourceLocation;
using clang::SourceManager;
using clang::SrcMgr::C_User;
using clang::TargetInfo;
using clang::TargetOptions;
using clang::TU_Module;

#include <json/json.h>
using Json::Value;
using Json::arrayValue;
using Json::objectValue;

#include <llvm/Support/Host.h>

#include <set>
#include <string>
#include <vector>
using std::cout;
using std::endl;
using std::ostringstream;
using std::set;
using std::string;
using std::vector;

#include "Consumer.h"
#include "optionparser.h"
#include "PrecompilerOptionAST.h"
using PrecompilerOptionAST::additionalIncludePaths;
using PrecompilerOptionAST::additionalPredefinedMacros;

#include "utils.h"



void FunctionTestDataFile::clearCollections() 
{
   _projectDescription.clear();
   
   if ( _unitFunctionTestCollection.get() != nullptr){
      _unitFunctionTestCollection->clear();
   }
   
   if ( _mockFunctionTestCollection.get() != nullptr){
      _mockFunctionTestCollection->clear();
   }
}


void FunctionTestDataFile::initCollections(const FunctionDeclKeySetMap   &funcDeclsMap,
                               const FunctionDeclKeySetMap   &mockDeclsMap)
{
//    clearCollections();

   _unitFunctionTestCollection = std::make_shared<UnitFunctionTestCollection>();
   _unitFunctionTestCollection->init(funcDeclsMap);
   
   _mockFunctionTestCollection = std::make_shared<MockFunctionTestCollection>();
   _mockFunctionTestCollection->init(mockDeclsMap);
}


void FunctionTestDataFile::deSerializeJson(const Json::Value &jsonRoot, const void* )
{
   //load json defined mocks
   const Value& mocksRoot = jsonRoot["mocks"];
   if( mocksRoot.empty() ){
      cout << "functions to mock not found in input json file: " << endl;
   }
   _mockFunctionTestCollection->deSerializeJson(mocksRoot);
   
   //load json defined unit/module tests
   const Value& funcsRoot = jsonRoot["funcs"];
   if( funcsRoot.empty() ){
      cout << "functions to test not found in input json file: " << endl;
   }
   _unitFunctionTestCollection->deSerializeJson(funcsRoot);
   
}


void FunctionTestDataFile::serializeJson(Value &jsonRoot) const
{ 
   jsonRoot["desc"] = Value(objectValue);
   _projectDescription.serializeJson(jsonRoot["desc"]);
   
   //init projectDescription up to the
   jsonRoot["funcs"] = Value(arrayValue);
   _unitFunctionTestCollection->serializeJson(jsonRoot["funcs"]);
   
   jsonRoot["mocks"] = Value(arrayValue);
   _mockFunctionTestCollection->serializeJson(jsonRoot["mocks"]);
}


void FunctionTestDataFile::computeAST(void)
{
   //get the files from command line args or from the json file "desc" : {"files"}
   _projectDescription.getFromOptionParser();

   //create the fare source to allow more than one source file
   _projectDescription.createFakeSource();
   
   // CompilerInstance
   // create DiagnosticOptions (not used ?)
   // creare DiagnosticEngine
   // Set default target triple (standard common block)
   DiagnosticOptions diagnosticOptions;
   _compiler.createDiagnostics();
   std::shared_ptr<TargetOptions> targetOptions =  std::make_shared<TargetOptions>();
   targetOptions->Triple = llvm::sys::getDefaultTargetTriple();
   TargetInfo *targetInfo = TargetInfo::CreateTargetInfo(_compiler.getDiagnostics(), targetOptions);
   _compiler.setTarget(targetInfo);

   // create FileManager
   _compiler.createFileManager();
   FileManager &fileMgr = _compiler.getFileManager();

   // create SourceManager
   _compiler.createSourceManager(fileMgr);
   SourceManager &SourceMgr = _compiler.getSourceManager();

   // get a reference to HeaderSearchOptions and add the include paths
   HeaderSearchOptions &searchOpts = _compiler.getHeaderSearchOpts();

   for( const path &p : _projectDescription.getIncludePaths() ) {
      cout << p.string() << endl;
      searchOpts.AddPath(p.string().c_str(), Quoted, false, false);
   }
   
   for( const path &p : _projectDescription.getSourcesPaths() ) {
      cout << p.string() << endl;
      searchOpts.AddPath(p.string().c_str(), Quoted, false, false);
   }
   

   // additional include search paths
   for (int i = 0; i < LIST_SIZE(additionalIncludePaths); ++i) {
      searchOpts.AddPath(additionalIncludePaths[i], CSystem, false, false);
   }

   // create Preprocessor (Preprocessor is initialized with all the CompilerInstance include paths)
   _compiler.createPreprocessor(TU_Module);
   Preprocessor &preProcessor = _compiler.getPreprocessor();

   // additional predefined macros needed for Compiling
   ostringstream additonalPredefMacrosStream;

   // get default clang 3.5 preprocessor predefined
   additonalPredefMacrosStream << preProcessor.getPredefines() << "\n";

   for (int i = 0; i < LIST_SIZE(additionalPredefinedMacros); ++i) {
      additonalPredefMacrosStream << "#define " << additionalPredefinedMacros[i] << "\n";
   }

   preProcessor.setPredefines(additonalPredefMacrosStream.str());

   // Set the main file handled by the source manager to the input file.
   const FileEntry *pFile = fileMgr.getFile(_projectDescription.getInputFileName());
   SourceMgr.setMainFileID(SourceMgr.createFileID(pFile, SourceLocation(), C_User));

   _compiler.createASTContext();

   // create UTBuilder consumer
   Consumer astConsumer(&_compiler.getASTContext(), _projectDescription.getAllFileNames());//pass source files

   // clear the results before parsing the AST
   results::get().clear();

   // Parse the AST and execute all the visitors
   ParseAST(_compiler.getPreprocessor(), &astConsumer, _compiler.getASTContext());
   
   //collections init
   initCollections( FunctionsToUnitTest::get().declKeySetMap, FunctionsToMock::get().declKeySetMap);
}
