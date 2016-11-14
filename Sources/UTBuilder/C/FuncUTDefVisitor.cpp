
#include "FuncUTDefVisitor.h"

#include <boost/filesystem.hpp>
#include <clang/Basic/SourceManager.h>
#include <clang/AST/ASTContext.h>

#include "Results.h"
#include "Utils.h"
#include "optionparser.h"

FuncUTDefVisitor::FuncUTDefVisitor(clang::ASTContext*              context, 
                                   const std::vector<std::string>& fileNames,
				   ASTinfo& info 
 				 )
   : Visitor(context, fileNames, info)
{
}



bool FuncUTDefVisitor::VisitDecl(clang::Decl *decl)
{
   const clang::FunctionDecl *func = llvm::dyn_cast<clang::FunctionDecl>(decl);

   if (func == nullptr) {
      return true;
   }

   if (func->hasBody()) {
      // get declaration source location
      const std::string declSrcFile = Utils::getDeclSourceFile(func, _context->getSourceManager());
      const std::string funcName = func->getNameAsString();
      
      // check if the funcDecl is in the input argument file
      for (const auto & fileName : _fileNames){
         
         if (declSrcFile.find(fileName) == std::string::npos) {
//             funcDecl is NOT in the input argument file"Sources/CurrencyTools/C/currency-convert.c"
            continue;
         }
         
         
         if ( OptionParser::get().isFunctionToTestEnabled() ) {
            /* one more check: is the funcDecl in the command line Option Parser -functions list ? */
            /* initialized only once */
            static std::set<std::string>  functionToTestFromOptionParser = OptionParser::get().getFunctionsToTest();
            if ( functionToTestFromOptionParser.find(funcName) == functionToTestFromOptionParser.end() ) {
               continue;
            }
         }

         // add to map with an empty set
//          FunctionsToUnitTest::get().declKeySetMap[func] = FunctionDeclSet();
//          FunctionsToUnitTest::get().nameDeclMap[funcName] = func;
	 
	 _info.addFunctionToUnitTest(func);
         
      }
   }

   return true;
}


FuncUTDeclVisitor::FuncUTDeclVisitor(clang::ASTContext*               context,
                                     const std::vector<std::string>&  fileNames,
				     ASTinfo& info
				    )
   : Visitor(context, fileNames, info)
{}


bool FuncUTDeclVisitor::VisitDecl(clang::Decl *decl)
{
   const clang::FunctionDecl *func = llvm::dyn_cast<clang::FunctionDecl>(decl);

   if (func == nullptr) {
      return true;
   }

   if (!func->hasBody()) {
      // get declaration source location
      const std::string declSrcFile = Utils::getDeclSourceFile(func, _context->getSourceManager());

//       for (auto funcToUnitTest : FunctionsToUnitTest::get().declKeySetMap) {
      for (auto funcToUnitTest : _info.getFunctionsToUnitTestMap() ) {
         const clang::FunctionDecl *funcDecl = funcToUnitTest.first;
         // check if the funcDecl is in the input argument file
         if (funcDecl->getNameInfo().getName().getAsString() == func->getNameInfo().getName().getAsString()) {
            boost::filesystem::path p(declSrcFile);
            
// 	    results::get().includesForUnitTest.insert(p.filename().string());
	    
	    _info.addIncludeFile(p.filename().string());
	    
            break;
         }
      }
   }

   return true;
}

