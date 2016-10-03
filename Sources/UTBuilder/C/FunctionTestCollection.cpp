
#include "FunctionTestCollection.h"


FunctionTestContent *FunctionTestCollection::ASTfind(const std::string &key)
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
//    for (std::map< std::string, FunctionTestContent>::const_iterator iter = _dataAST.begin(); iter!=_dataAST.end(); ++iter ) {
   for (const std::pair<std::string, FunctionTestContent>& iter : _dataAST) {
      const FunctionTestContent &funcParams = iter.second;
      funcParams.serializeJson(jsonRoot);
   }
}


void FunctionTestCollection::serializeJson(Json::Value &jsonRoot) const
{
//    for (std::map< std::string, FunctionTestContent>::const_iterator iter = _dataJson.begin(); iter!=_dataJson.end(); ++iter ) {
   for (const std::pair<std::string, FunctionTestContent>& iter : _dataJson) {
      const FunctionTestContent &funcParams = iter.second;
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

   FunctionTestContent funcTestContentJson;
   
   for (unsigned int i = 0; i < size; ++i) {
      const Json::Value& value = jsonRoot[i];
      const Json::Value funcNameValue = value.get("_name", "");
      const std::string funcName = funcNameValue.asString();

      const FunctionTestContent *funcTestContentAST = ASTfind(funcName);

      if (funcTestContentAST) {
         funcTestContentJson.deSerializeJson( funcTestContentAST, value);
         _dataJson[funcTestContentJson.getName()] = funcTestContentJson;
      }
      else {
//        TO DO error message  
      }
   }
}


void UnitFunctionTestCollection::init(const FunctionDeclKeySetMap   &funcDeclsMap)
{
//    FunctionTestContent funcParamsStruct;

   for (const auto& iter : funcDeclsMap) {
      const clang::FunctionDecl *funcDecl = iter.first;
      const std::string name = funcDecl->getNameAsString();
      const std::set<const clang::FunctionDecl *> &mockDeclSet = iter.second;

      _dataAST[name] = FunctionTestContent(funcDecl, mockDeclSet);
   }
}


void MockFunctionTestCollection::init(const FunctionDeclKeySetMap   &funcDeclsMap)
{

   for (const auto& iter : funcDeclsMap) {
      const clang::FunctionDecl *funcDecl = iter.first;
      const std::string name = funcDecl->getNameAsString();

      _dataAST[name] = FunctionTestContent(funcDecl);
   }

}

