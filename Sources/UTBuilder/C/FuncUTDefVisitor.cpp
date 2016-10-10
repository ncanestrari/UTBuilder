
#include "FuncUTDefVisitor.h"

#include <boost/filesystem.hpp>
#include <clang/Basic/SourceManager.h>
#include <clang/AST/ASTContext.h>

#include "Results.h"
#include "utils.h"


FuncUTDefVisitor::FuncUTDefVisitor(clang::ASTContext*              context, 
                                   const std::vector<std::string>& fileNames)
   : Visitor(context, fileNames)
{}



bool FuncUTDefVisitor::VisitDecl(clang::Decl *decl)
{
   const clang::FunctionDecl *func = llvm::dyn_cast<clang::FunctionDecl>(decl);

   if (func == nullptr) {
      return true;
   }

   if (func->hasBody()) {
      // get declaration source location
      const std::string declSrcFile = utils::getDeclSourceFile(func, _context->getSourceManager());

      // check if the funcDecl is in the input argument file
      bool found = false;
      for (const auto & fileName : _fileNames){
         if (declSrcFile.find(fileName) != std::string::npos) {
            // add to map with an empty set
            FunctionsToUnitTest::get().declKeySetMap[func] = FunctionDeclSet();
            FunctionsToUnitTest::get().nameDeclMap[func->getNameAsString()] = func;
         }
      }
   }

   return true;
}


FuncUTDeclVisitor::FuncUTDeclVisitor(clang::ASTContext*               context,
                                     const std::vector<std::string>&  fileNames)
   : Visitor(context, fileNames)
{}


bool FuncUTDeclVisitor::VisitDecl(clang::Decl *decl)
{
   const clang::FunctionDecl *func = llvm::dyn_cast<clang::FunctionDecl>(decl);

   if (func == nullptr) {
      return true;
   }

   if (!func->hasBody()) {
      // get declaration source location
      const std::string declSrcFile = utils::getDeclSourceFile(func, _context->getSourceManager());

      for (auto funcToUnitTest : FunctionsToUnitTest::get().declKeySetMap) {
         const clang::FunctionDecl *funcDecl = funcToUnitTest.first;
         // check if the funcDecl is in the input argument file
         if (funcDecl->getNameInfo().getName().getAsString() == func->getNameInfo().getName().getAsString()) {
            boost::filesystem::path p(declSrcFile);
            results::get().includesForUnitTest.insert(p.filename().string());
            break;
         }
      }
   }

   return true;
}

