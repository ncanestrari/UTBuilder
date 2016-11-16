#include "ASTinfo.h"

#include <clang/AST/Decl.h>

#include <iostream>


const std::set<const clang::FunctionDecl *> ASTinfo::_emptySet = std::set<const clang::FunctionDecl *>();
 
void FunctionsToUnitTest::clear()
{
   _declKeySetMap.clear();
   _nameDeclMap.clear();
}


void FunctionsToMock::clear()
{
   _declKeySetMap.clear();
   _nameDeclMap.clear();
}

/*
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
*/


void ASTinfo::clear()
{
   _functionsToUnitTest._nameDeclMap.clear();
   _functionsToUnitTest._declKeySetMap.clear();
   
   _functionsToMock._nameDeclMap.clear();
   _functionsToMock._declKeySetMap.clear();

   _structureDecls.clear();
   _typedefNameDecls.clear();
   _includeFilesForUnitTest.clear();
   _functionDeclTypes.clear();
 
}


void ASTinfo::addFunctionToUnitTest(const clang::FunctionDecl* funcDecl, const clang::FunctionDecl* mockFuncDecl)
{
   // check if the function decl is already added
//    auto iter = _functionsToUnitTest.declKeySetMap.find(funcDecl);
//    if ( iter == _functionsToUnitTest.declKeySetMap.end() ) {
      _functionsToUnitTest._nameDeclMap[funcDecl->getNameAsString()] = funcDecl;
      _functionsToUnitTest._declKeySetMap[funcDecl] = FunctionDeclSet();   
//    }
//    else {
//       
//    }
//    
//    auto mockIter = _functionsToMock.declKeySetMap.find(mockFuncDecl);
//    if ( mockIter  == _functionsToMock.declKeySetMap.end() ) {
//    // add the function decl to mock
//    // TO DO: NOT IMPLEMETED YET
//    }
}


void ASTinfo::addFunctionToMock(const clang::FunctionDecl* mockFuncDecl, const clang::FunctionDecl* callerFuncDecl) 
{
   
   FunctionDeclKeySetMap::iterator iter = _functionsToMock._declKeySetMap.find(mockFuncDecl);
   
   if (iter != _functionsToMock._declKeySetMap.end() ) {
      // already in the map. add the caller
      iter->second.insert(callerFuncDecl);
   } 
   else {
      _functionsToMock._declKeySetMap[mockFuncDecl] = FunctionDeclSet();
      _functionsToMock._nameDeclMap[mockFuncDecl->getNameAsString()] = mockFuncDecl;
   }
   
   if (_functionsToUnitTest._declKeySetMap.size() > 0) {
      iter = _functionsToUnitTest._declKeySetMap.find(callerFuncDecl);
      if (iter != _functionsToUnitTest._declKeySetMap.end()) {
	 iter->second.insert(mockFuncDecl);
      } else {
	 // just a temporary check to debug
	 std::cout <<  mockFuncDecl->getNameAsString() << ": mock function caller ("<< callerFuncDecl->getNameAsString() <<") doesn't need to be tested\n";
      }
   }
}


const std::set<const clang::FunctionDecl *>& ASTinfo::getMockCalleesForFunction(const clang::FunctionDecl * funcDecl)
{ 
   auto iter = _functionsToUnitTest._declKeySetMap.find(funcDecl);
   if ( iter == _functionsToUnitTest._declKeySetMap.end() )
   {
      return ASTinfo::_emptySet;
   }
   else {
      return iter->second;
   }
}
 
const std::set<const clang::FunctionDecl *>& ASTinfo::getFunctionCallersForMock(const clang::FunctionDecl * funcDecl)
{ 
   auto iter = _functionsToMock._declKeySetMap.find(funcDecl);
   if ( iter == _functionsToMock._declKeySetMap.end() )
   {
      return ASTinfo::_emptySet;
   }
   else {
      return iter->second;
   }
}


void ASTinfo::fillFunctionQualTypes(void)
{
   _functionDeclTypes.clear();

   //canonical Types: http://clang.llvm.org/docs/InternalsManual.html#canonical-types
   for (auto funcToMock : _functionsToMock._declKeySetMap) {
      const clang::FunctionDecl *funcDecl = funcToMock.first;
      const clang::QualType returnType = funcDecl->getReturnType();
      _functionDeclTypes.insert(returnType->getCanonicalTypeInternal().getTypePtrOrNull());

      const int numParms = funcDecl->getNumParams();
      for (int i = 0; i < numParms; ++i) {
         const clang::ParmVarDecl *_currentParam = funcDecl->getParamDecl(i);
         const clang::QualType qualType = _currentParam->getOriginalType();

         _functionDeclTypes.insert(qualType->getCanonicalTypeInternal().getTypePtrOrNull());
      }
   }

   for (auto functionToTest : _functionsToUnitTest._declKeySetMap) {
      const clang::FunctionDecl *funcDecl = functionToTest.first;
      const clang::QualType returnType = funcDecl->getReturnType();
//       results::get().functionDeclTypes.insert(returnType->getCanonicalTypeInternal().getTypePtrOrNull());
      _functionDeclTypes.insert(returnType->getCanonicalTypeInternal().getTypePtrOrNull());
      const int numParms = funcDecl->getNumParams();

      for (int i = 0; i < numParms; ++i) {
         const clang::ParmVarDecl *_currentParam = funcDecl->getParamDecl(i);
         const clang::QualType qualType = _currentParam->getOriginalType();

         _functionDeclTypes.insert(qualType->getCanonicalTypeInternal().getTypePtrOrNull());
      }
   }
}
