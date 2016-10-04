#ifndef _UTBuilder_FunctionTestCollection_h__
#define _UTBuilder_FunctionTestCollection_h__


#include <string>
#include <map>

#include "Serializable.h"
#include "FunctionTestContent.h"


#include "Results.h"


// namespace clang {
// class FunctionDecl;
// }


class FunctionTestCollection : public Serializable {
public:
   
   FunctionTestCollection() {}
   
   FunctionTestCollection(const FunctionTestCollection&) = delete;
   
   ~FunctionTestCollection() {}
   
   // override and fill the _data map
   virtual void init(const FunctionDeclKeySetMap   &funcDeclsMap) {}
   
   void clear()
   {
      _dataFromAST.clear();
      _dataFromJson.clear();
   }

   FunctionTestContent *findContentFromAST(const std::string &key);

//    Serializable interface
   virtual void serializeJson(Json::Value &jsonRoot ) const override;

   virtual void deSerializeJson(const Json::Value &jsonRoot, const void *refData = nullptr) override;
   

   const std::map< std::string, FunctionTestContent> &dataFromAST(void)
   {
      return _dataFromAST;
   }
   
   const std::map< std::string, FunctionTestContent> &dataFromJson(void) const
   {
      return _dataFromJson;
   }

protected:

   std::map< std::string, FunctionTestContent> _dataFromAST;
   std::map< std::string, FunctionTestContent> _dataFromJson;

};



class UnitFunctionTestCollection : public FunctionTestCollection 
{
public:

   UnitFunctionTestCollection() {}
   ~UnitFunctionTestCollection() {}
   
   virtual void init(const FunctionDeclKeySetMap   &funcDeclsMap) override;

};



class MockFunctionTestCollection : public FunctionTestCollection
{
public:
   
   MockFunctionTestCollection() {}
   ~MockFunctionTestCollection() {}

   virtual void init(const FunctionDeclKeySetMap   &funcDeclsMap) override;

};



#endif // _UTBuilder_FunctionTestCollection_h__

