
#include "JsonReader.h"


#include <json/json.h>

#include <iostream>
#include <fstream>
#include <memory>

#include <boost/filesystem.hpp>

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
bool JsonReader::parse( const string  &fileName)
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
      
      _unitTestData.deSerializeJson(jsonRoot);
   }
   
   jsonFile.close();
   
   
   
   
   
   Json::Value jsonRoot;
 
   _unitTestData.serializeJson(jsonRoot);

   std::ofstream outputFile;
   boost::filesystem::path tmp("Tests");
   boost::filesystem::create_directory(tmp);
   boost::filesystem::path ci = boost::filesystem::current_path();
   boost::filesystem::current_path(tmp);
   
   std::string outputFileName ="gloabalsOutputTest.json";
   outputFile.open(outputFileName, std::fstream::out);
   if( !outputFile.is_open() ){
      throw std::logic_error("cannot open file" + outputFileName + "\n");
   }
   outputFile << jsonRoot;
   outputFile.close();

   std::cout << "file written: " << outputFileName << std::endl;
   boost::filesystem::current_path(ci);
   
   return true;
}

