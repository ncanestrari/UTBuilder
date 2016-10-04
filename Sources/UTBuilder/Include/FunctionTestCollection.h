#ifndef _UTBuilder_FunctionTestCollection_h__
#define _UTBuilder_FunctionTestCollection_h__


#include <string>
#include <map>

#include "FunctionTestContent.h"


#include "Results.h"



class FunctionTestCollection {
public:
   // override and fill the _data map
   virtual void init(const FunctionDeclKeySetMap   &funcDeclsMap) {}
   
   void clear()
   {
      _dataAST.clear();
      _dataJson.clear();
   }

   FunctionTestContent *ASTfind(const std::string &key);

   void serializeAST(Json::Value &jsonRoot) const;
   void serializeJson(Json::Value &jsonRoot) const;
//    void deSerialize(Json::Value &jsonRoot);
   void deSerializeJson(const Json::Value &jsonRoot);

   const std::map< std::string, FunctionTestContent> &dataAST(void)
   {
      return _dataAST;
   }
   
   const std::map< std::string, FunctionTestContent> &dataJson(void)
   {
      return _dataJson;
   }

protected:

   std::map< std::string, FunctionTestContent> _dataAST;
   std::map< std::string, FunctionTestContent> _dataJson;

};



class UnitFunctionTestCollection : public FunctionTestCollection //, public Singleton<UnitFunctionTestCollection> {
{
public:

   virtual void init(const FunctionDeclKeySetMap   &funcDeclsMap) override;

};



class MockFunctionTestCollection : public FunctionTestCollection //, public Singleton<MockFunctionTestCollection> {
{
public:

   virtual void init(const FunctionDeclKeySetMap   &funcDeclsMap) override;

};



#endif // _UTBuilder_FunctionTestCollection_h__

