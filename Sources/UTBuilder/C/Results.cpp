
#include "Results.h"


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

