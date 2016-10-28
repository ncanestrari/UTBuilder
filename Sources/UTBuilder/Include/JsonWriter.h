#ifndef _UTBuilder_JsonWriter_h__
#define _UTBuilder_JsonWriter_h__


#include "FunctionTestDataFile.h"
#include "UnitTestData.h"

#include <string>


class JsonWriter {

public:
   JsonWriter(const FunctionTestDataFile& dataFile, const UnitTestData& unitTestData);
   ~JsonWriter() {}

   void templateFile(const std::string &fileName);
   

private:

   const FunctionTestDataFile& _dataFile;
   const UnitTestData& _unitTestData;

};

#endif // _UTBuilder_JsonWriter_h__ 
 
