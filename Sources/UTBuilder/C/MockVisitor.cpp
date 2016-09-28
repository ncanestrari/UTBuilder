
#include "MockVisitor.h"

#include <boost/filesystem.hpp>
#include <clang/Basic/SourceManager.h>
#include <clang/AST/ASTContext.h>

#include "Results.h"
#include "utils.h"


MockVisitor::MockVisitor(clang::ASTContext   *context,
                         std::string          fileName)
   : Visitor(context, fileName)
{
}


bool MockVisitor::VisitDecl(clang::Decl *decl)
{
   const clang::FunctionDecl *func = llvm::dyn_cast<clang::FunctionDecl>(decl);

   if (func == nullptr) {
      return true;
   }

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
   const std::string srcFile = utils::getStmtSourceFile(funcCall, srcMgr);

   // get function declaration source location
   const std::string declSrcFile = utils::getDeclSourceFile(funcDecl, srcMgr);

   // check if the statement is in the input argument file
   if (declSrcFile.find(_fileName) != std::string::npos) {
      // this function doesn't need to be mocked but tested
      return true;
   }


   // TODO: fix this
   // temporary check
   // WEAK CHECK if declaration is in CommercialCode path
   if (declSrcFile.find("CommercialCode") == std::string::npos) {
      // this is (probably) a system function
      return true;
   }

   // mock this function
   FunctionDeclKeySetMap::iterator iter = FunctionsToMock::get().declKeySetMap.find(funcDecl);
   if (iter != FunctionsToMock::get().declKeySetMap.end()) {
      // already in the map. add the caller
      iter->second.insert(_lastFuncDecl);
   } else {
      FunctionsToMock::get().declKeySetMap[funcDecl] = FunctionDeclSet();
      FunctionsToMock::get().nameDeclMap[funcDecl->getNameAsString()] = funcDecl;
   }

   if (FunctionsToUnitTest::get().declKeySetMap.size() > 0) {
      iter = FunctionsToUnitTest::get().declKeySetMap.find(_lastFuncDecl);
      if (iter != FunctionsToUnitTest::get().declKeySetMap.end()) {
         iter->second.insert(funcDecl);
      } else {
         // just a temporary check to debug
         std::cout <<  funcDecl->getNameAsString() << " mock function caller doesn't need to be tested\n";
      }
   }

   return true;
}
