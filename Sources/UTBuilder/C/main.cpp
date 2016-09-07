
#include "Action.h"
#include "Consumer.h"
#include "utils.h"
#include "Results.h"
#include "writer.h"

#include <clang/AST/Expr.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Frontend/ASTUnit.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Lex/HeaderSearch.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Parse/ParseAST.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>
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

   vector<string> includePaths;
   utils::getIncludePaths( comcode+package, includePaths );
   
   clang::CompilerInstance compiler;
   clang::DiagnosticOptions diagnosticOptions;
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
   
   return 0;
}
