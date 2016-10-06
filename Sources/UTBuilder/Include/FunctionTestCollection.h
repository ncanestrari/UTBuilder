#ifndef _UTBuilder_FunctionTestCollection_h__
#define _UTBuilder_FunctionTestCollection_h__


#include <string>
#include <map>

#include "FunctionTestContent.h"


#include "Results.h"



class FunctionTestCollection {
public:
   
   FunctionTestCollection() {}
   FunctionTestCollection(const FunctionTestCollection&) = delete;
   ~FunctionTestCollection() {}
   
   void clear()
   {
      _dataAST.clear();
      _dataJson.clear();
   }

   FunctionTestContent *findContentFromAST(const std::string &key);

   void serializeJson(Json::Value &jsonRoot) const;
   void deSerializeJson(const Json::Value &jsonRoot);

   //getters
   const std::map< std::string, FunctionTestContent> &dataAST(void){ return _dataAST; }
   const std::map< std::string, FunctionTestContent> &dataJson(void) const { return _dataJson; }

protected:

   std::map< std::string, FunctionTestContent> _dataAST;
   std::map< std::string, FunctionTestContent> _dataJson;

};



class UnitFunctionTestCollection : public FunctionTestCollection //, public Singleton<UnitFunctionTestCollection> {
{
public:

   UnitFunctionTestCollection() {}
   ~UnitFunctionTestCollection() {}
   
   void init(const FunctionDeclKeySetMap   &funcDeclsMap);

};



class MockFunctionTestCollection : public FunctionTestCollection //, public Singleton<MockFunctionTestCollection> {
{
public:
   
   MockFunctionTestCollection() {}
   ~MockFunctionTestCollection() {}

   void init(const FunctionDeclKeySetMap   &funcDeclsMap);

};



#endif // _UTBuilder_FunctionTestCollection_h__

