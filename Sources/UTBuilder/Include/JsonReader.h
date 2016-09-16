#ifndef _UTBuilder_JsonReader_h__ 
#define _UTBuilder_JsonReader_h__


#include <json/json.h>

#include <string>


class JsonReader 
{
public:
   JsonReader(const std::string&            fileName );
  
   ~JsonReader() {}
   
   bool parse(void);
   
private:
   
   

   
   const std::string  _fileName;
   
   Json::Value _funcsRoot;
   Json::Value _mocksRoot;
   
   static const char* _mocksFileNameSuffix;
   static const char* _funcsFileNameSuffix;
   
};

#endif // _UTBuilder_JsonReader_h__