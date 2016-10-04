
#include "FunctionTestCollection.h"


FunctionTestContent *FunctionTestCollection::findContentFromAST(const std::string &key)
{
   FunctionTestContent *funParamPtr = nullptr;
   std::map< std::string, FunctionTestContent>::iterator iter = _dataFromAST.find(key);

   if (iter != _dataFromAST.end()) {
      funParamPtr = &(iter->second);
   }

   return funParamPtr;
}


void FunctionTestCollection::serializeJson(Json::Value &jsonRoot) const
{
   
//    serialize only data from AST
   for (const std::pair<std::string, FunctionTestContent>& iter : _dataFromAST) {
      const FunctionTestContent &funcParams = iter.second;
      funcParams.serializeJson(jsonRoot);
   }
}


void FunctionTestCollection::deSerializeJson(const Json::Value &jsonRoot, const void*  )
{
//    const Json::Value funcs = jsonRoot["funcs"];
   const unsigned int size = jsonRoot.size();

   FunctionTestContent funcTestContentJson;
   
   for (unsigned int i = 0; i < size; ++i) {
      const Json::Value& value = jsonRoot[i];
      const Json::Value funcNameValue = value.get("_name", "");
      const std::string funcName = funcNameValue.asString();

      const FunctionTestContent *funcTestContentAST = findContentFromAST(funcName);

      if (funcTestContentAST) {
         funcTestContentJson.deSerializeJson( value, funcTestContentAST);
         _dataFromJson[funcTestContentJson.getName()] = funcTestContentJson;
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

      _dataFromAST[name] = FunctionTestContent(funcDecl, mockDeclSet);
   }
}


void MockFunctionTestCollection::init(const FunctionDeclKeySetMap   &funcDeclsMap)
{

   for (const auto& iter : funcDeclsMap) {
      const clang::FunctionDecl *funcDecl = iter.first;
      const std::string name = funcDecl->getNameAsString();

      _dataFromAST[name] = FunctionTestContent(funcDecl);
   }

}

