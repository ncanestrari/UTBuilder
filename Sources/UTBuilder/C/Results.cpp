#include "Results.h"



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

