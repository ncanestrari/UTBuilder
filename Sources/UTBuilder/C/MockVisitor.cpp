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
   results::get().functionsToMock.insert(funcDecl);

   return true;
}
