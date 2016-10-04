
#include "FunctionTestDataFile.h"


FunctionTestDataFile::FunctionTestDataFile()
{
   
}
   
FunctionTestDataFile::~FunctionTestDataFile()
{
}
   
   
void FunctionTestDataFile::clearCollections() 
{
   if ( _unitFunctionTestCollection.get() != nullptr)
      _unitFunctionTestCollection->clear();
   
   if ( _mockFunctionTestCollection.get() != nullptr)
      _mockFunctionTestCollection->clear();
}

void FunctionTestDataFile::initCollections(const FunctionDeclKeySetMap   &funcDeclsMap,
                               const FunctionDeclKeySetMap   &mockDeclsMap)
{
   clearCollections();

   _unitFunctionTestCollection = std::make_shared<UnitFunctionTestCollection>();
   _unitFunctionTestCollection->init(funcDeclsMap);
   
   _mockFunctionTestCollection = std::make_shared<MockFunctionTestCollection>();
   _mockFunctionTestCollection->init(mockDeclsMap);
}

void FunctionTestDataFile::deSerializeJson(const Json::Value &jsonRoot, const void* )
{
   
   const Json::Value& mocksRoot = jsonRoot["mocks"];
   if (mocksRoot.empty() )
      std::cout << "functions to mock not found in input json file: " << std::endl;
   
   
   const Json::Value& funcsRoot = jsonRoot["funcs"];
   if (funcsRoot.empty() )
      std::cout << "functions to test not found in input json file: " << std::endl;
   
   _unitFunctionTestCollection->deSerializeJson(funcsRoot);
   _mockFunctionTestCollection->deSerializeJson(mocksRoot);
   
}

void FunctionTestDataFile::serializeJson(Json::Value &jsonRoot) const
{
   jsonRoot["mocks"] = Json::Value(Json::arrayValue);
   _mockFunctionTestCollection->serializeJson(jsonRoot["mocks"]);
   
   jsonRoot["funcs"] = Json::Value(Json::arrayValue);
   _unitFunctionTestCollection->serializeJson(jsonRoot["funcs"]);
}