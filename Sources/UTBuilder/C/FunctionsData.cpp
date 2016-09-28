
#include "FunctionsData.h"


FuncParamsStruct *FunctionsData::find(const std::string &key)
{
   FuncParamsStruct *funParamPtr = nullptr;
   std::map< std::string, FuncParamsStruct>::iterator iter = _dataAST.find(key);

   if (iter != _dataAST.end()) {
      funParamPtr = &(iter->second);
   }

   return funParamPtr;
}


void FunctionsData::serializeAST(Json::Value &jsonRoot) const
{
   for (auto iter : _dataAST) {
      FuncParamsStruct &funcParams = iter.second;
      funcParams.serialize(jsonRoot["funcs"]);
   }
}


void FunctionsData::serializeJson(Json::Value &jsonRoot) const
{
   for (auto iter : _dataJson) {
      FuncParamsStruct &funcParams = iter.second;
      funcParams.serialize(jsonRoot["funcs"]);
   }
}


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
}


void FunctionsData::deSerializeJson(const Json::Value &jsonRoot)
{
   const Json::Value funcs = jsonRoot["funcs"];
   const unsigned int size = funcs.size();

   for (unsigned int i = 0; i < size; ++i) {
      const Json::Value funcNameValue = funcs[i].get("_name", "");
      const std::string funcName = funcNameValue.asString();

      FuncParamsStruct *funcParams = find(funcName);
      if (funcParams) {
         funcParams->deSerializeJson(_dataAST[funcName], funcs[i]);
      }

      _dataJson[funcParams->getName()] = *funcParams;
   }
}


void UnitTestFunctionsData::init(const FunctionDeclKeySetMap   &funcDeclsMap)
{
   FuncParamsStruct funcParamsStruct;

   for (auto iter : funcDeclsMap) {
      const clang::FunctionDecl *funcDecl = iter.first;
      const std::set<const clang::FunctionDecl *> &mockDeclSet = iter.second;

      funcParamsStruct.init(funcDecl, mockDeclSet);
      _dataAST[funcParamsStruct.getName()] = funcParamsStruct;
   }
}


void MockFunctionsData::init(const FunctionDeclKeySetMap   &funcDeclsMap)
{
   FuncParamsStruct funcParamsStruct;

   for (auto iter : funcDeclsMap) {
      const clang::FunctionDecl *funcDecl = iter.first;
      funcParamsStruct.init(funcDecl);
      _dataAST[funcParamsStruct.getName()] = funcParamsStruct;
   }

}

