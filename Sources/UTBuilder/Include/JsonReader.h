
#ifndef _UTBuilder_JsonReader_h__
#define _UTBuilder_JsonReader_h__


#include "FunctionTestDataFile.h"
#include "UnitTestData.h"

#include <string>


class JsonReader {

public:
   JsonReader(UnitTestData& unitTestData);
   ~JsonReader() {}

   bool parse(FunctionTestDataFile &data, const std::string &fileName);
//    bool parse(FunctionTestCollection &funcsData, const std::string &fileName);

private:

   std::string  _fileName;
   /* This are not used ... */
   Json::Value _funcsRoot;
   Json::Value _mocksRoot;
   Json::Value _descRoot;
   
   UnitTestData& _unitTestData;

};

#endif // _UTBuilder_JsonReader_h__

