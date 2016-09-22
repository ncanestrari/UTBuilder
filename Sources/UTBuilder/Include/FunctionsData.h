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
   
   
   void clear() { _data.clear(); }
   
   FuncParamsStruct* find(const std::string& key);
   
   void serialize(Json::Value& jsonRoot) const;
   
   void deSerialize(Json::Value& jsonRoot);
   
   const std::map< std::string, FuncParamsStruct>& data(void) { return _data;}
   
   
   
protected:
   
   std::map< std::string, FuncParamsStruct> _data;
   
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

