#include "JsonReader.h"

#include "FuncParamsStructure.h"

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
   

   
   std::map< const std::string, const clang::DeclaratorDecl* > funcParamStructMap;
   std::vector< std::shared_ptr<FuncParamsStruct> > funcParamStructVector( _funcsRoot["funcs"].size() );
   std::shared_ptr<FuncParamsStruct> funcs = std::make_shared<FuncParamsStruct>(  );
   
   for ( auto func : _funcsRoot["funcs"])
   {
      
   }
   
   return true;
}
