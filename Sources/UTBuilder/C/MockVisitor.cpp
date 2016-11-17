
#include "MockVisitor.h"

#include <boost/filesystem.hpp>
#include <clang/Basic/SourceManager.h>
#include <clang/AST/ASTContext.h>

#include "ASTinfo.h"
#include "Utils.h"

#include <vector>

MockVisitor::MockVisitor(clang::ASTContext*              context,
                         const std::vector<std::string>& fileNames,
                         ClangCompiler& compiler )
   : Visitor(context, fileNames, compiler)
{
}


bool MockVisitor::VisitDecl(clang::Decl *decl)
{
   const clang::FunctionDecl *func = llvm::dyn_cast<clang::FunctionDecl>(decl);

   if (func == nullptr) {
      return true;
   }

   // keep the last function definition. this will be the caller of the mock functions
   if (func->hasBody()) {
      _lastFuncDecl = func;
   }

   return true;
}


bool MockVisitor::VisitCallExpr(clang::CallExpr *funcCall)
{
   // get the declaration of the funcCall
   const clang::FunctionDecl *funcDecl = funcCall->getDirectCallee();
   const clang::SourceManager &srcMgr = _context->getSourceManager();

   // function call source location
   const std::string srcFile = Utils::getStmtSourceFile(funcCall, srcMgr);

   // get function declaration source location
   const std::string declSrcFile = Utils::getDeclSourceFile(funcDecl, srcMgr);

   // check if the statement is in the input argument file

   for (const auto & fileName : _fileNames){
      if (declSrcFile.find(fileName) != std::string::npos) {
         // this function doesn't need to be mocked but tested
         return true;
      }
   }
   

   // TODO: fix this
   // temporary check
   // WEAK CHECK if declaration is in CommercialCode path
   if (declSrcFile.find(boost::filesystem::current_path().string()) == std::string::npos) {
      // this is (probably) a system function
      return true;
   }

   // check if the caller (_lastFuncDecl) needs to be tested
   const std::string callerFuncName = _lastFuncDecl->getNameAsString();
   
   
   // check if the caller (_lastFuncDecl) needs to be tested
   if ( _info->getFunctionsToUnitTest().find(callerFuncName) == _info->getFunctionsToUnitTest().end() ) {
      return true;
   }
   
   // mock this function
   _info->addFunctionToMock(funcDecl, _lastFuncDecl);
   

   return true;
}
