
#include "FunctionTestCollection.h"


FunctionTestContent *FunctionTestCollection::find(const std::string &key)
{
   FunctionTestContent *funParamPtr = nullptr;
   std::map< std::string, FunctionTestContent>::iterator iter = _dataAST.find(key);

   if (iter != _dataAST.end()) {
      funParamPtr = &(iter->second);
   }

   return funParamPtr;
}


void FunctionTestCollection::serializeAST(Json::Value &jsonRoot) const
{
   for (auto iter : _dataAST) {
      FunctionTestContent &funcParams = iter.second;
      funcParams.serializeJson(jsonRoot);
   }
}


void FunctionTestCollection::serializeJson(Json::Value &jsonRoot) const
{
   for (auto iter : _dataJson) {
      FunctionTestContent &funcParams = iter.second;
      funcParams.serializeJson(jsonRoot);
   }
}

/*
void FunctionsData::deSerialize(Json::Value &jsonRoot)
{
   const Json::Value funcs = jsonRoot["funcs"];
   const unsigned int size = funcs.size();

   for (unsigned int i = 0; i < size; ++i) {
      const Json::Value funcNameValue = funcs[i].get("_name", "");
      const std::string funcName = funcNameValue.asString();

      FuncParamsStruct *funcParams = find(funcName);
      if (funcParams) {
         funcParams->deSerialize(funcs[i]);
      }
   }
}*/


void FunctionTestCollection::deSerializeJson(const Json::Value &jsonRoot)
{
//    const Json::Value funcs = jsonRoot["funcs"];
   const unsigned int size = jsonRoot.size();

   for (unsigned int i = 0; i < size; ++i) {
      const Json::Value funcNameValue = jsonRoot[i].get("_name", "");
      const std::string funcName = funcNameValue.asString();

      FunctionTestContent *funcParams = find(funcName);
      if (funcParams) {
         funcParams->deSerializeJson(_dataAST[funcName], jsonRoot[i]);
      }

      _dataJson[funcParams->getName()] = *funcParams;
   }
}


void UnitFunctionTestCollection::init(const FunctionDeclKeySetMap   &funcDeclsMap)
{
   FunctionTestContent funcParamsStruct;

   for (auto iter : funcDeclsMap) {
      const clang::FunctionDecl *funcDecl = iter.first;
      const std::set<const clang::FunctionDecl *> &mockDeclSet = iter.second;

      funcParamsStruct.init(funcDecl, mockDeclSet);
      _dataAST[funcParamsStruct.getName()] = funcParamsStruct;
   }
}


void MockFunctionTestCollection::init(const FunctionDeclKeySetMap   &funcDeclsMap)
{
   FunctionTestContent funcParamsStruct;

   for (auto iter : funcDeclsMap) {
      const clang::FunctionDecl *funcDecl = iter.first;
      funcParamsStruct.init(funcDecl);
      _dataAST[funcParamsStruct.getName()] = funcParamsStruct;
   }

}

