
#include "JsonReader.h"

#include "FuncParamsStructure.h"
#include "FunctionsData.h"

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
JsonReader::JsonReader()
{}


/** 
 * Parse the Json file named fileName, into the a FunctionsData file
 * 
 * @param[out]    FunctionsData funcsData 
 * @param[in]     string        fileName
 * 
 * @return        true if process works, false otherwise 
 */
bool JsonReader::parse(FunctionsData &funcsData,
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
      jsonFile >> _funcsRoot;
      funcsData.deSerializeJson(_funcsRoot);
   }
   jsonFile.close();

   return true;
}

