
#include "DataFile.h"
#include <json/json.h>

using std::cout;
using std::endl;
using Json::Value;
using Json::arrayValue;

void DataFile::clearCollections() 
{
   unitFunctionTestCollection.clear();
   mockFunctionTestCollection.clear();
   projectDescription.clear();
}

void DataFile::initCollections(const FunctionDeclKeySetMap   &funcDeclsMap)
{
   clearCollections();
   unitFunctionTestCollection.init(funcDeclsMap);
   mockFunctionTestCollection.init(funcDeclsMap);
}

void DataFile::deSerializeJson(const Value &jsonRoot)
{
   const Value& descRoot = jsonRoot["desc"];
   if( descRoot.empty() ){
      cout << "project description not found in input json file: " << endl;
   }
   projectDescription.deserializeJson(descRoot);
   
   const Value& funcsRoot = jsonRoot["funcs"];
   if( funcsRoot.empty() ){
      cout << "functions to test not found in input json file: " << endl;
   }
   
   const Value& mocksRoot = jsonRoot["mocks"];
   if( mocksRoot.empty() ){
      cout << "functions to mock not found in input json file: " << endl;
   }
   
   
   unitFunctionTestCollection.deSerializeJson(funcsRoot);
   mockFunctionTestCollection.deSerializeJson(mocksRoot);
}

void DataFile::serializeAST(Value &jsonRoot) const
{
   jsonRoot["funcs"] = Value(arrayValue);
   unitFunctionTestCollection.serializeAST(jsonRoot["funcs"]);
   
   jsonRoot["mocks"] = Value(arrayValue);
   mockFunctionTestCollection.serializeAST(jsonRoot["mocks"]);
}