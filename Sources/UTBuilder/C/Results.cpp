#include "Results.h"

#include <clang/AST/Decl.h>

#include <iostream>



void FunctionsToUnitTest::clear()
{
   declKeySetMap.clear();
   nameDeclMap.clear();
}


void FunctionsToMock::clear()
{
   declKeySetMap.clear();
   nameDeclMap.clear();
}


results &results::get(void)
{
   static results instance;
   return instance;
};


void results::clear()
{
   structDecls.clear();
   typedefNameDecls.clear();
   includesForUnitTest.clear();
   functionDeclTypes.clear();
}



void ASTinfo::clear()
{
   _functionsToUnitTest.nameDeclMap.clear();
   _functionsToUnitTest.declKeySetMap.clear();
   
   _functionsToMock.nameDeclMap.clear();
   _functionsToMock.declKeySetMap.clear();

   _structureDecls.clear();
   _typedefNameDecls.clear();
   _includeFilesForUnitTest.clear();
   _functionDeclTypes.clear();
 
}


void ASTinfo::addFunctionToUnitTest(const clang::FunctionDecl* funcDecl)
{
   _functionsToUnitTest.nameDeclMap[funcDecl->getNameAsString()] = funcDecl;
   _functionsToUnitTest.declKeySetMap[funcDecl] = FunctionDeclSet();
}


void ASTinfo::addFunctionToMock(const clang::FunctionDecl* mockFuncDecl, const clang::FunctionDecl* callerFuncDecl) 
{
   
   FunctionDeclKeySetMap::iterator iter = _functionsToMock.declKeySetMap.find(mockFuncDecl);
   
   if (iter != _functionsToMock.declKeySetMap.end() ) {
      // already in the map. add the caller
      iter->second.insert(callerFuncDecl);
   } 
   else {
      _functionsToMock.declKeySetMap[mockFuncDecl] = FunctionDeclSet();
      _functionsToMock.nameDeclMap[mockFuncDecl->getNameAsString()] = mockFuncDecl;
   }
   
   if (_functionsToUnitTest.declKeySetMap.size() > 0) {
      iter = _functionsToUnitTest.declKeySetMap.find(callerFuncDecl);
      if (iter != _functionsToUnitTest.declKeySetMap.end()) {
	 iter->second.insert(mockFuncDecl);
      } else {
	 // just a temporary check to debug
	 std::cout <<  mockFuncDecl->getNameAsString() << ": mock function caller ("<< callerFuncDecl->getNameAsString() <<") doesn't need to be tested\n";
      }
   }
}
