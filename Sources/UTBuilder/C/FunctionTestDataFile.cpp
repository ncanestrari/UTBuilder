
#include "FunctionTestDataFile.h"



void FunctionTestDataFile::clearCollections() 
{
   unitFunctionTestCollection.clear();
   mockFunctionTestCollection.clear();
}

void FunctionTestDataFile::initCollections(const FunctionDeclKeySetMap   &funcDeclsMap,
                               const FunctionDeclKeySetMap   &mockDeclsMap)
{
   clearCollections();

   unitFunctionTestCollection.init(funcDeclsMap);
   mockFunctionTestCollection.init(mockDeclsMap);
}

void FunctionTestDataFile::deSerializeJson(const Json::Value &jsonRoot)
{
   
   const Json::Value& mocksRoot = jsonRoot["mocks"];
   if (mocksRoot.empty() )
      std::cout << "functions to mock not found in input json file: " << std::endl;
   
   
   const Json::Value& funcsRoot = jsonRoot["funcs"];
   if (funcsRoot.empty() )
      std::cout << "functions to test not found in input json file: " << std::endl;
   
   unitFunctionTestCollection.deSerializeJson(funcsRoot);
   mockFunctionTestCollection.deSerializeJson(mocksRoot);
   
}

void FunctionTestDataFile::serializeJson(Json::Value &jsonRoot) const
{
   jsonRoot["mocks"] = Json::Value(Json::arrayValue);
   mockFunctionTestCollection.serializeJson(jsonRoot["mocks"]);
   
   jsonRoot["funcs"] = Json::Value(Json::arrayValue);
   unitFunctionTestCollection.serializeJson(jsonRoot["funcs"]);
}