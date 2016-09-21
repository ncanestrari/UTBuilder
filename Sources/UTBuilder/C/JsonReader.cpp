#include "JsonReader.h"

#include "FuncParamsStructure.h"
#include "Results.h"

#include <iostream>
#include <fstream>
#include <memory>

const char* JsonReader::_mocksFileNameSuffix = "-mocks.json";
const char* JsonReader::_funcsFileNameSuffix = "-unitTest.json";


JsonReader::JsonReader(const std::string&            fileName )
: _fileName(fileName)
{
}

bool JsonReader::parse(void)
{
   
   
   std::ifstream jsonFile;
   
   std::string funcsFileNameJson = _fileName + _funcsFileNameSuffix;
   jsonFile.open(funcsFileNameJson);
   
   if ( jsonFile.is_open() == false)
   {
      std::cout << "json file not found: " << funcsFileNameJson << std::endl;
      return false;
   }
   
   jsonFile >> _funcsRoot;
   jsonFile.close();
   
   
   std::string mocksFileNameJson = _fileName + _mocksFileNameSuffix;
   jsonFile.open(mocksFileNameJson);
   
   if ( jsonFile.is_open() == false)
   {
      std::cout << "json file not found: " << mocksFileNameJson << std::endl;
      return false;
   }
   
   jsonFile >> _mocksRoot;
   jsonFile.close();
   
   
   
   const Json::Value funcs = _funcsRoot["funcs"];
   const unsigned int size = funcs.size();
   

   for ( unsigned int i = 0; i < size; ++i )
   {
      const Json::Value funcNameValue = funcs[i].get("_name", "");
      const std::string funcName = funcNameValue.asString();
      
      auto iter = FunctionParams::get().structs.find(funcName);
      
      if ( iter != FunctionParams::get().structs.end() )
      {
         iter->second.deSerializeJson( funcs[i] );
      }
      
   }

   return true;
}
