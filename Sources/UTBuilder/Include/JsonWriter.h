#ifndef _UTBuilder_JsonWriter_h__
#define _UTBuilder_JsonWriter_h__


#include "UnitTestData.h"

#include <string>


class JsonWriter {

public:
   JsonWriter(const UnitTestData& unitTestData);
   ~JsonWriter() {}

   void templateFile(const std::string &fileName);
   

private:

   const UnitTestData& _unitTestData;

};

#endif // _UTBuilder_JsonWriter_h__ 
 
