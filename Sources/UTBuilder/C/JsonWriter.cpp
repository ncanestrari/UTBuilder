#include "JsonWriter.h"

#include <json/json.h>

#include <iostream>
#include <fstream>




JsonWriter::JsonWriter(const FunctionTestDataFile& dataFile)
: _dataFile(dataFile)
{
}



void JsonWriter::templateFile(const std::string  &fileName)
{
   Json::Value jsonRoot;
   
   _dataFile.serializeJson(jsonRoot);


   std::ofstream outputFile;
   std::string outputFileName = fileName + ".json";
   outputFile.open(outputFileName, std::fstream::out);
   outputFile << jsonRoot;
   outputFile.close();

   std::cout << "file written: " << outputFileName << std::endl;
}


