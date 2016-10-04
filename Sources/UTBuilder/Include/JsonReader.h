
#ifndef _UTBuilder_JsonReader_h__
#define _UTBuilder_JsonReader_h__


#include "FunctionTestDataFile.h"

#include <string>


class JsonReader {

public:
   JsonReader();
   ~JsonReader() {}

   bool parse(FunctionTestDataFile &data, const std::string &fileName);
//    bool parse(FunctionTestCollection &funcsData, const std::string &fileName);

private:

   std::string  _fileName;

   Json::Value _funcsRoot;
   Json::Value _mocksRoot;

};

#endif // _UTBuilder_JsonReader_h__

