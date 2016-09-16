#include "Results.h"



FunctionsToUnitTest& FunctionsToUnitTest::get(void) {
  
   static FunctionsToUnitTest instance;
   return instance;
};
   
   
void FunctionsToUnitTest::clear() 
{
   declKeySetMap.clear();
   nameDeclMap.clear();
}



FunctionsToMock& FunctionsToMock::get(void) {
  
   static FunctionsToMock instance;
   return instance;
};
   
   
void FunctionsToMock::clear() 
{
   declKeySetMap.clear();
   nameDeclMap.clear();
}


results& results::get(void) {
  
   static results instance;
   return instance;
};
   
   
void results::clear() 
{
   functionsToMockMap.clear();
   functionsToUnitTestMap.clear();
   structDecls.clear();
   typedefNameDecls.clear();
   includesForUnitTest.clear();
   functionDeclTypes.clear();     
}

