#include "Results.h"



results& results::get(void) {
  
   static results instance;
   return instance;
};
   
   
void results::clear() 
{
   functionsToMock.clear();
   functionsToUnitTest.clear();
   structDecls.clear();
   typedefNameDecls.clear();
   includesForUT.clear();
   functionDeclTypes.clear();     
}

