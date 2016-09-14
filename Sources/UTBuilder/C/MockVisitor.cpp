#include "MockVisitor.h"

#include <boost/filesystem.hpp>
#include <clang/Basic/SourceManager.h>
#include <clang/AST/ASTContext.h>

#include "Results.h"
#include "utils.h"


MockVisitor::MockVisitor(clang::ASTContext*   context,
                         std::string          fileName) 
: Visitor(context, fileName)
{

}


bool MockVisitor::VisitDecl(clang::Decl* decl)
{
   const clang::FunctionDecl* func = llvm::dyn_cast<clang::FunctionDecl>(decl);

   if (func == nullptr)
      return true;

   if ( func->hasBody() ){
     
      _lastFuncDecl = func;
   } 

   return true;
}

bool MockVisitor::VisitCallExpr(clang::CallExpr* funcCall)
{
   // get the declaration of the funcCall
   const clang::FunctionDecl* funcDecl = funcCall->getDirectCallee();

   //std::cout << funcDecl->getNameInfo().getName().getAsString() << "\t";


   const clang::SourceManager& srcMgr = _context->getSourceManager();

  // function call source location
   const std::string srcFile = utils::getStmtSourceFile( funcCall, srcMgr );
   
   // get function declaration source location      
   const std::string declSrcFile = utils::getDeclSourceFile( funcDecl, srcMgr );
   
   // check if the statement is in the input argument file 
   if ( declSrcFile.find( _fileName) != std::string::npos )
   {
      // this function doesn't need to be mocked but tested
      //std::cout << "this function doesn't need to be mocked but tested" << std::endl;
      //_functionToUnitTest.insert(funcDecl);
      return true;
   }


   // TO DO: fix this
   // temporary check
   // WEAK CHECK if declaration is in CommercialCode path
   if ( declSrcFile.find( "CommercialCode") == std::string::npos )
   {
      // this is (probably) a system function
      //std::cout << "this is (probably) a system function" << std::endl;
      return true;
   }
   
   // mock this function
   //std::cout << "accepted" << std::endl;
//    results::get().functionsToMock.insert(funcDecl);

   FunctionDeclKeySetMap::iterator iter = results::get().functionsToMockMap.find(funcDecl); 
   if ( iter != results::get().functionsToMockMap.end() )
   {
      // already in the map. add the caller
      iter->second.insert(_lastFuncDecl);
   }
   else{
      // insert funcDecl with an empty set of callers
      results::get().functionsToMockMap[funcDecl] = FunctionDeclSet();
   }
   
   
   if ( results::get().functionsToUnitTestMap.size() > 0 )
   {
      iter = results::get().functionsToUnitTestMap.find(_lastFuncDecl); 
      if ( iter != results::get().functionsToUnitTestMap.end() )
      {
         iter->second.insert( funcDecl );
      }
      else{
         // just a temporary check to debug
         std::cout <<  funcDecl->getNameAsString() << " mock function caller doesn't need to be tested\n";
      }
   }
   
   return true;
}
