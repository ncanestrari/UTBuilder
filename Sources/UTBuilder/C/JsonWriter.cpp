#include "JsonWriter.h"

#include <json/json.h>

#include <iostream>
#include <fstream>
#include <stdexcept>



JsonWriter::JsonWriter(const FunctionTestDataFile& dataFile, const UnitTestData& unitTestData)
: _dataFile(dataFile)
, _unitTestData(unitTestData)
{
}



void JsonWriter::templateFile(const std::string  &fileName)
{
   Json::Value jsonRoot;
 
   _dataFile.serializeJson(jsonRoot);

   std::ofstream outputFile;
   std::string outputFileName = fileName + ".json";
   outputFile.open(outputFileName, std::fstream::out);
   if( !outputFile.is_open() ){
      throw std::logic_error("cannot open file" + outputFileName + "\n");
   }
   outputFile << jsonRoot;
   outputFile.close();

   std::cout << "file written: " << outputFileName << std::endl;
   
   
   
   jsonRoot.clear();
   _unitTestData.serializeJson(jsonRoot);

   /*std::ofstream outputFile;*/
   /*std::string*/ outputFileName = fileName + "UnitTestData" + ".json";
   outputFile.open(outputFileName, std::fstream::out);
   if( !outputFile.is_open() ){
      throw std::logic_error("cannot open file" + outputFileName + "\n");
   }
   outputFile << jsonRoot;
   outputFile.close();

   std::cout << "file written: " << outputFileName << std::endl;
   
}


