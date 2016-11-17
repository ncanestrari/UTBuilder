#include "JsonWriter.h"

#include <json/json.h>

#include <iostream>
#include <fstream>
#include <stdexcept>

#include <boost/filesystem.hpp>
using boost::filesystem::create_directory;
using boost::filesystem::current_path;
using boost::filesystem::exists;
using boost::filesystem::is_directory;
using boost::filesystem::path;


JsonWriter::JsonWriter(const UnitTestData& unitTestData)
: _unitTestData(unitTestData)
{
}



void JsonWriter::templateFile(const std::string  &fileName)
{
   Json::Value jsonRoot;
 
   _unitTestData.serializeJson(jsonRoot);

   std::ofstream outputFile;
   path tmp = path("Tests");
   create_directory(tmp);
   path ci = current_path();
   current_path(tmp);
   
   std::string outputFileName = fileName + ".json";
   outputFile.open(outputFileName, std::fstream::out);
   if( !outputFile.is_open() ){
      throw std::logic_error("cannot open file" + outputFileName + "\n");
   }
   outputFile << jsonRoot;
   outputFile.close();

   std::cout << "file written: " << outputFileName << std::endl;
   current_path(ci);
}


