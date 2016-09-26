
#include "Action.h"
#include "Consumer.h"
#include "utils.h"
#include "Results.h"
#include "writer.h"
#include "JsonReader.h"

#include <iostream>


#include <clang/Parse/ParseAST.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/HeaderSearch.h>
#include <llvm/Support/Host.h>


#include <fstream>

// additional paths needed for compiling (system or compiler specific)
static const char* additionalIncludePaths[] =
{
   "/usr/include",
   "/usr/lib64/clang/3.5.0/include"
};

// additional predefined macros needed for preprocessing
static const char* additionalPredefinedMacros[] = 
{
   "__UNIX64__",
   "__64BIT__",
   "__LINUX64__",
   "SLES_10",
   "NDEBUG"
};


static const char* mocksFileNameSuffix = "-mocks.json";
static const char* funcsFileNameSuffix = "-unitTest.json";

   
   
int main(int argc, const char *argv[])
{
   
   // get include search paths from CommercialCode and Packages needed to compile file argv[1]
   std::string fileNamePath(argv[1]);
   std::vector<std::string> include_folders;
   std::string comcode = utils::extractCommercialCodePath(fileNamePath);
   std::string package = utils::extractPackagePath(fileNamePath);
   
   std::vector<std::string> includePaths;
   utils::getIncludePaths( comcode+package, includePaths );
   
   
   
   // CompilerInstance
   clang::CompilerInstance compiler;
   
   // create DiagnosticOptions (not used ?)
   clang::DiagnosticOptions diagnosticOptions;
   
   // creare DiagnosticEngine
   compiler.createDiagnostics();
  
  
   // Set default target triple (standard common block)
   std::shared_ptr<clang::TargetOptions> targetOptions =  std::make_shared<clang::TargetOptions>();
   targetOptions->Triple = llvm::sys::getDefaultTargetTriple();
   clang::TargetInfo *targetInfo = clang::TargetInfo::CreateTargetInfo(compiler.getDiagnostics(), targetOptions);
   compiler.setTarget(targetInfo);
  
   
   // create FileManager 
   compiler.createFileManager();
   clang::FileManager& fileMgr = compiler.getFileManager();
   
    // create SourceManager
   compiler.createSourceManager( fileMgr );
   clang::SourceManager& SourceMgr = compiler.getSourceManager();
    
   
   // add included search paths to compiler instance HeaderSearchOpts
   
   // get a reference to HeaderSearchOptions and add the include paths
   clang::HeaderSearchOptions& searchOpts = compiler.getHeaderSearchOpts();
 
   for ( auto path : includePaths )
   {
      searchOpts.AddPath( path, clang::frontend::Quoted, false, false );
   }
   
   // additional include search paths
   const size_t numAdditionalIncludePaths = sizeof(additionalIncludePaths)/sizeof(*additionalIncludePaths);
   for (int i=0; i<numAdditionalIncludePaths; ++i)
   {
      searchOpts.AddPath(additionalIncludePaths[i], clang::frontend::CSystem, false, false );
   }

   
   // create Preprocessor (Preprocessor is initialized with all the CompilerInstance include paths)
   compiler.createPreprocessor(clang::TU_Module);
   clang::Preprocessor& preProcessor = compiler.getPreprocessor();
    
   // additional predefined macros needed for Compiling
   std::ostringstream additonalPredefMacrosStream;
   
   // get default clang 3.5 preprocessor predefined
   additonalPredefMacrosStream << preProcessor.getPredefines() << "\n";
   
   const size_t numAdditionalPredefinedMacros = sizeof(additionalPredefinedMacros)/sizeof(*additionalPredefinedMacros);
   for (int i=0; i<numAdditionalPredefinedMacros; ++i )
   {
       additonalPredefMacrosStream << "#define " << additionalPredefinedMacros[i] << "\n";
   }
   
   preProcessor.setPredefines( additonalPredefMacrosStream.str() );
   
   
   // Set the main file handled by the source manager to the input file.
   const clang::FileEntry *pFile = fileMgr.getFile( fileNamePath );
   SourceMgr.setMainFileID(  SourceMgr.createFileID( pFile, clang::SourceLocation(), clang::SrcMgr::C_User) );
   
   //compiler.getDiagnosticClient().BeginSourceFile(compiler.getLangOpts(), &preProcessor );
   
   
   compiler.createASTContext();
   
   // create UTBuilder consumer
   Consumer astConsumer( &compiler.getASTContext(), fileNamePath );
   
   // clear the results before parsing the AST
   results::get().clear();
   
   // Parse the AST and execute all the visitors
   clang::ParseAST(compiler.getPreprocessor(), &astConsumer, compiler.getASTContext());
   
   
   
   UnitTestFunctionsData::get().clear();
   UnitTestFunctionsData::get().init( FunctionsToUnitTest::get().declKeySetMap );
   
   MockFunctionsData::get().clear();
   MockFunctionsData::get().init( FunctionsToMock::get().declKeySetMap );
   
   
   Writer writer(fileNamePath, SourceMgr );
   bool writeJsonExampleFile = true;
   if ( writeJsonExampleFile )
   {
      // write the json example files
      writer.createFiles();
   }
   
   
   
   JsonReader reader;
   reader.parse( UnitTestFunctionsData::get(), fileNamePath + funcsFileNameSuffix );
   reader.parse( MockFunctionsData::get(), fileNamePath + mocksFileNameSuffix );
   
   
   Json::Value jsonRoot;
   UnitTestFunctionsData::get().serializeJson( jsonRoot );
   
   
   
   std::ofstream outputFile;   
   std::string outputFileName = "fileNamePath-temp.json";   
   outputFile.open( outputFileName, std::fstream::out );
   outputFile << jsonRoot;
   outputFile.close();
   
//    std::cout << "file written: " << outputFileName << std::endl;
      
   
   writer.createUnitTestFile();
   
   //compiler.getDiagnosticClient().EndSourceFile();
 
   
   return 0;
}
