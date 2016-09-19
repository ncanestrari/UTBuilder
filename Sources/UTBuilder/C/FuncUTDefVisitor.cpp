
#include "FuncUTDefVisitor.h"

#include <boost/filesystem.hpp>
#include <clang/Basic/SourceManager.h>
#include <clang/AST/ASTContext.h>

#include "Results.h"
#include "utils.h"


FuncUTDefVisitor::FuncUTDefVisitor(clang::ASTContext* context, std::string fileName)
: Visitor(context, fileName)
{
  
}



bool FuncUTDefVisitor::VisitDecl(clang::Decl* decl)
{

  const clang::FunctionDecl* func = llvm::dyn_cast<clang::FunctionDecl>(decl);

   if (func == nullptr)
      return true;

   if ( func->hasBody() ){
     
      // get declaration source location      
      const std::string declSrcFile = utils::getDeclSourceFile( func, _context->getSourceManager());
      
      // check if the funcDecl is in the input argument file
      if ( declSrcFile.find( _fileName) != std::string::npos )
      {
//          results::get().functionsToUnitTest.insert(func);
         // add to map with an empty set
//          results::get().functionsToUnitTestMap[func] = FunctionDeclSet();
//          results::get().functionsToUnitTestName[func->getNameAsString()] = func;
         FunctionsToUnitTest::get().declKeySetMap[func] = FunctionDeclSet();
         FunctionsToUnitTest::get().nameDeclMap[func->getNameAsString()] = func;
      }
   } 

   return true;
}


FuncUTDeclVisitor::FuncUTDeclVisitor(clang::ASTContext* context, std::string fileName)
: Visitor(context, fileName)
{
  
}


bool FuncUTDeclVisitor::VisitDecl(clang::Decl* decl)
{

  const clang::FunctionDecl* func = llvm::dyn_cast<clang::FunctionDecl>(decl);

   if (func == nullptr)
      return true;

   if ( !func->hasBody() ){
     
      // get declaration source location
      const std::string declSrcFile = utils::getDeclSourceFile( func, _context->getSourceManager());
      
      // check if the funcDecl is in the input argument file    
      for ( auto funcToUnitTest : FunctionsToUnitTest::get().declKeySetMap ){
         
         const clang::FunctionDecl* funcDecl = funcToUnitTest.first;
         
         if( funcDecl->getNameInfo().getName().getAsString() == func->getNameInfo().getName().getAsString() ){
            boost::filesystem::path p(declSrcFile);
            results::get().includesForUnitTest.insert(p.filename().string());
            break;
         }
      }
   
   } 

   return true;
}

