
#ifndef _UTBuilder_JsonReader_h__
#define _UTBuilder_JsonReader_h__


#include "UnitTestData.h"

#include <string>


class JsonReader {

public:
   JsonReader(UnitTestData& unitTestData);
   ~JsonReader() {}

   bool parse(const std::string &fileName);
//    bool parse(FunctionTestCollection &funcsData, const std::string &fileName);

private:

   std::string  _fileName;
   
   UnitTestData& _unitTestData;

};

#endif // _UTBuilder_JsonReader_h__

