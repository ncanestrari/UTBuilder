#ifndef _UTBuilder_JsonWriter_h__
#define _UTBuilder_JsonWriter_h__


#include "FunctionTestDataFile.h"

#include <string>


class JsonWriter {

public:
   JsonWriter(const FunctionTestDataFile& dataFile);
   ~JsonWriter() {}

   void templateFile(const std::string &fileName);
   

private:

   const FunctionTestDataFile& _dataFile;

};

#endif // _UTBuilder_JsonWriter_h__ 
 
