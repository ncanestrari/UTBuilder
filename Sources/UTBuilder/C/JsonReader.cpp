#include "JsonReader.h"

#include "FuncParamsStructure.h"
#include "FunctionsData.h"

#include <iostream>
#include <fstream>
#include <memory>


JsonReader::JsonReader(){}

bool JsonReader::parse(FunctionsData &funcsData, const std::string &fileName)
{

   _fileName = fileName;

   std::ifstream jsonFile;

   jsonFile.open(_fileName);

   if (jsonFile.is_open() == false) {
      std::cout << "json file not found: " << _fileName << std::endl;
      return false;
   } else {

      std::cout << "reading json file: " << _fileName << std::endl;

      jsonFile >> _funcsRoot;

      funcsData.deSerializeJson(_funcsRoot);
   }

   jsonFile.close();

   return true;
}
