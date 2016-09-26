#ifndef _UTBuilder_FunctionsData_h__
#define _UTBuilder_FunctionsData_h__

#include <string>
// #include <set>
#include <map>

#include "FuncParamsStructure.h"

// for the signleton template
#include "Results.h"


namespace clang {
   class FunctionDecl;
//    class RecordDecl;
//    class TypedefNameDecl;
//    class Type;
}



class FunctionsData
{
public:
   
   // override and fill the _data map
   virtual void init(const FunctionDeclKeySetMap   &funcDeclsMap) {}
   
   
   void clear() { _dataAST.clear(); }
   
   FuncParamsStruct* find(const std::string& key);
   
   void serializeAST(Json::Value& jsonRoot) const;

   void serializeJson(Json::Value& jsonRoot) const;
   
   void deSerialize(Json::Value& jsonRoot);
   
   void deSerializeJson( const Json::Value& jsonRoot);
   
   const std::map< std::string, FuncParamsStruct>& dataAST(void) { return _dataAST;}
   
   const std::map< std::string, FuncParamsStruct>& dataJson(void) { return _dataJson;}
   
   
protected:
   
   std::map< std::string, FuncParamsStruct> _dataAST;
   
   std::map< std::string, FuncParamsStruct> _dataJson;
   
};



class UnitTestFunctionsData : public FunctionsData, public Singleton<UnitTestFunctionsData>
{
   
public:
   
   virtual void init(const FunctionDeclKeySetMap   &funcDeclsMap ) override;
   
};



class MockFunctionsData : public FunctionsData, public Singleton<MockFunctionsData>
{
public:
   
   virtual void init(const FunctionDeclKeySetMap   &funcDeclsMap ) override;
   
};



#endif // _UTBuilder_FunctionsData_h__

