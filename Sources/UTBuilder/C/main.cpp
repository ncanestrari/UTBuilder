
#include "Action.h"
#include "Consumer.h"
#include "utils.h"
#include "Results.h"
#include "writer.h"

#include <iostream>

#include <clang/AST/Expr.h>
#include <clang/Frontend/ASTUnit.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>

#include <clang/Parse/ParseAST.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/HeaderSearch.h>
#include <llvm/Support/Host.h>

using std::string;
using std::vector;



static llvm::cl::OptionCategory testToolCategory("test tool options");
static llvm::cl::extrahelp commonHelp( clang::tooling::CommonOptionsParser::HelpMessage);
static llvm::cl::extrahelp additionalHelp("\n .... some more help");

int main(int argc, const char *argv[])
{
   string fileNamePath(argv[1]);
   vector<string> include_folders;
   string comcode = utils::extractCommercialCodePath(fileNamePath);
   string package = utils::extractPackagePath(fileNamePath);

   /*
   vector<string> argument_list;
   utils::getIncludePaths(argument_list, comcode+package);

   // allocating a pointer of pointer to const char: (char const*) *
   
   const vector<string>  macroDef({ "D__UNIX64__", "-D__64BIT__", "-D__LINUX64__", "-D_LP64", "-DSLES_10", "-DNDEBUG"});
   argument_list.insert(argument_list.end(), macroDef.begin(), macroDef.end());
   argument_list.push_back("-c");
   argument_list.push_back(fileNamePath);
   const size_t numbIncludePaths = argument_list.size();
   const char**  argumentsVec = new const char*[numbIncludePaths];
   
   for ( int i=0; i<numbIncludePaths; ++i )
   {
      argumentsVec[i] = argument_list[i].c_str();
   }
   */
   
   vector<string> includePaths;
   utils::getIncludePaths( comcode+package, includePaths );
   
   // CompilerInstance
   clang::CompilerInstance compiler;
   
   // create DiagnosticOptions
   clang::DiagnosticOptions diagnosticOptions;
   
   // creare DiagnosticEngine
   compiler.createDiagnostics();
  
  
   // Set default target triple
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
    
   
   // add system paths
   /*
   llvm::IntrusiveRefCntPtr<clang::HeaderSearchOptions> headerSearchOptions( new clang::HeaderSearchOptions());
   clang::HeaderSearch headerSearch( headerSearchOptions, 
                              compiler.getSourceManager(),
                              compiler.getDiagnostics(),
                              compiler.getLangOpts(),
                              targetInfo);
    
   */
   
   // get a reference to HeaderSearchOptions and add the include paths
   clang::HeaderSearchOptions& searchOpts = compiler.getHeaderSearchOpts();
 
   for ( auto path : includePaths )
   {
      searchOpts.AddPath( path, clang::frontend::Quoted, false, false );
   }
   
   // additional paths
   searchOpts.AddPath("/usr/include", clang::frontend::CSystem, false, false );
   // weak path: compiler version 
   searchOpts.AddPath("/usr/lib64/clang/3.5.0/include", clang::frontend::CSystem, false, false );
   
   
   //const clang::HeaderSearchOptions& searchOptsConstRef = compiler.getHeaderSearchOpts();
   
   // create Preprocessor 
   compiler.createPreprocessor(clang::TU_Module);
   clang::Preprocessor& preProcessor = compiler.getPreprocessor();
   
   // get default clang 3.5 preprocessor predefined
   const std::string&  defaultPredef = preProcessor.getPredefines();
    
   // additional predefined to Compile 
   const std::string additonalPredef="#define __UNIX64__  \n \
                                      #define __64BIT__   \n \
                                      #define __LINUX64__ \n \
                                      #define SLES_10     \n \
                                      #define NDEBUG      \n ";

   
   preProcessor.setPredefines( defaultPredef + additonalPredef );
   
//    clang::PreprocessorOptions& preProcessorOptions = compiler.getPreprocessorOpts();
//    preProcessorOptions.UsePredefines = true;
//    preProcessorOptions.addMacroDef("__UNIX64__");
//    preProcessorOptions.addMacroDef("__64BIT__");
//    preProcessorOptions.addMacroDef("__LINUX64__");
//    preProcessorOptions.addMacroDef("_LP64");
//    preProcessorOptions.addMacroDef("SLES_10");
//    preProcessorOptions.addMacroDef("NDEBUG");   
   
   //clang::InitializePreprocessor( preProcessor, compiler.getPreprocessorOpts(), compiler.getFrontendOpts() );

   //clang::LangOptions langOpts;
   //clang::CompilerInvocation& invocation = compiler.getInvocation();
   //invocation.setLangDefaults(langOpts, clang::IK_C /*, clang::LangStandard::lang_c99*/ );
   
   
   // Set the main file handled by the source manager to the input file.
   const clang::FileEntry *pFile = fileMgr.getFile( fileNamePath );
   SourceMgr.setMainFileID(  SourceMgr.createFileID( pFile, clang::SourceLocation(), clang::SrcMgr::C_User) );
   compiler.getDiagnosticClient().BeginSourceFile(compiler.getLangOpts(), &preProcessor );
   
   
   
   compiler.createASTContext();
   
   // create UTBuilder consumer
   Consumer astConsumer( &compiler.getASTContext(), fileNamePath );
   
   // clear the results before parsing
   results::get().clear();
   
   // Parse the AST and execute all the visitors
   clang::ParseAST(compiler.getPreprocessor(), &astConsumer, compiler.getASTContext());
   
   // write the files
   Writer writer(fileNamePath, SourceMgr );
   writer.createFiles();
   
   
   //compiler.getDiagnosticClient().EndSourceFile();
  
  
   /*
   clang::tooling::CommonOptionsParser _optionParser(argc, argv, testToolCategory);
   clang::tooling::ClangTool _tool(_optionParser.getCompilations(), _optionParser.getSourcePathList());

   int result = _tool.run(clang::tooling::newFrontendActionFactory<Action>().get());
*/
   
   return 0;
}
