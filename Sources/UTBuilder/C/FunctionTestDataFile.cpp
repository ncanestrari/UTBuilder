
#include "FunctionTestDataFile.h"
#include <json/json.h>

using std::cout;
using std::endl;
using Json::Value;
using Json::arrayValue;

void FunctionTestDataFile::clearCollections() 
{
   unitFunctionTestCollection.clear();
   mockFunctionTestCollection.clear();
   projectDescription.clear();
}

void FunctionTestDataFile::initCollections(const FunctionDeclKeySetMap   &funcDeclsMap,
                               const FunctionDeclKeySetMap   &mockDeclsMap)
{
   clearCollections();
   unitFunctionTestCollection.init(funcDeclsMap);
   mockFunctionTestCollection.init(mockDeclsMap);
}

void FunctionTestDataFile::deSerializeJson(const Value &jsonRoot)
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

void FunctionTestDataFile::serializeAST(Value &jsonRoot) const
{
   jsonRoot["funcs"] = Value(arrayValue);
   unitFunctionTestCollection.serializeAST(jsonRoot["funcs"]);
   
   jsonRoot["mocks"] = Value(arrayValue);
   mockFunctionTestCollection.serializeAST(jsonRoot["mocks"]);
}