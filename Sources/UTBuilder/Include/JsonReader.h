
#ifndef _UTBuilder_JsonReader_h__
#define _UTBuilder_JsonReader_h__


#include <json/json.h>

#include "FunctionsData.h"

#include <string>


class JsonReader {

public:
   JsonReader();
   ~JsonReader() {}

   bool parse(FunctionsData &funcsData, const std::string &fileName);

private:

   std::string  _fileName;

   Json::Value _funcsRoot;
   Json::Value _mocksRoot;

};

#endif // _UTBuilder_JsonReader_h__

