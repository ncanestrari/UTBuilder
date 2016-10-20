
#include "JsonReader.h"

#include "FunctionTestDataFile.h"

#include <json/json.h>

#include <iostream>
#include <fstream>
#include <memory>

using std::cout;
using std::endl;
using std::ifstream;
using std::string;


/** 
 * Constructor of JsonReader is empty
 */
JsonReader::JsonReader(UnitTestData& unitTestData)
: _unitTestData(unitTestData)
{}


/** 
 * Parse the Json file named fileName, into the a FunctionsData file
 * 
 * @param[out]    FunctionsData funcsData 
 * @param[in]     string        fileName
 * 
 * @return        true if process works, false otherwise 
 */
bool JsonReader::parse(FunctionTestDataFile &data,
                       const string  &fileName)
{
   ifstream jsonFile;
   _fileName = fileName;

   jsonFile.open(_fileName);
   if (jsonFile.is_open() == false) {
      cout << "json file not found: " << _fileName << endl;
      return false;
   } else {
      cout << "reading json file: " << _fileName << endl;
      
      Json::Value jsonRoot;
      jsonFile >> jsonRoot;
      
      data.deSerializeJson(jsonRoot); 
      _unitTestData.deSerializeJson(jsonRoot);
   }
   
   jsonFile.close();
   
   return true;
}

