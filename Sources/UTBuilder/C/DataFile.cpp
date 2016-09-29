
#include "DataFile.h"



void DataFile::clearCollections() 
{
   unitFunctionTestCollection.clear();
   mockFunctionTestCollection.clear();
}

void DataFile::initCollections(const FunctionDeclKeySetMap   &funcDeclsMap)
{
   clearCollections();
   unitFunctionTestCollection.init(funcDeclsMap);
   mockFunctionTestCollection.init(funcDeclsMap);
}

void DataFile::deSerializeJson(const Json::Value &jsonRoot)
{
   const Json::Value& funcsRoot = jsonRoot["funcs"];
   if (funcsRoot.empty() )
      std::cout << "functions to test not found in input json file: " << std::endl;
   
   const Json::Value& mocksRoot = jsonRoot["mocks"];
   if (mocksRoot.empty() )
      std::cout << "functions to mock not found in input json file: " << std::endl;
   
   
   unitFunctionTestCollection.deSerializeJson(funcsRoot);
   mockFunctionTestCollection.deSerializeJson(mocksRoot);
   
}

void DataFile::serializeAST(Json::Value &jsonRoot) const
{
   jsonRoot["funcs"] = Json::Value(Json::arrayValue);
   unitFunctionTestCollection.serializeAST(jsonRoot["funcs"]);
   
   jsonRoot["mocks"] = Json::Value(Json::arrayValue);
   mockFunctionTestCollection.serializeAST(jsonRoot["mocks"]);
}