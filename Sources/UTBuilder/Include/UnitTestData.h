#ifndef _UTBuilder_UnitTestData_h__
#define _UTBuilder_UnitTestData_h__

#include "Serializable.h"
// #include "singleton.h"
#include "NameValueTypeNode.h"
#include "Results.h"

#include <set>

namespace clang{
   class FunctionDecl;
}


class UnitTestData :  public Serializable //, public Singleton<UnitTestData>
{

public:
   
//    UnitTestData( const clang::FunctionDecl *funcDecl,
//              const std::set<const clang::FunctionDecl *> &mockFuncs = std::set<const clang::FunctionDecl *>());
   
   UnitTestData(const FunctionDeclKeySetMap   &funcDeclsMap,
                const FunctionDeclKeySetMap   &mockDeclsMap);
   
   UnitTestData(const UnitTestData&) = delete;
   
   ~UnitTestData();


//    Serializable interface
   virtual void serializeJson(Json::Value &jsonRoot ) const override final;
   virtual void deSerializeJson(const Json::Value &jsonRoot, const void *funcTestData=nullptr) override final;


   NameValueNode* buildCollectionTree();
      
private:

   void clear(void);
   

   
   NameValueNode* buildDescTree();
   NameValueNode* buildMocksTree();
   NameValueNode* buildFunctionsTree();

//    NameValueNode* buildMockContentTree();
//    NameValueNode* buildFunctionContentTree();
   
   NameValueNode* buildMockContentTree(const clang::FunctionDecl *funcDecl,
                            const std::set<const clang::FunctionDecl *> &mockFuncs);
   
   NameValueNode* buildFunctionContentTree(const clang::FunctionDecl *funcDecl,
                            const std::set<const clang::FunctionDecl *> &mockFuncs);
   
   std::unique_ptr<NameValueNode> _tree;
   
   
   NameValueNode* buildTreeFromAST( const clang::FunctionDecl *funcDecl,
                                    const std::set<const clang::FunctionDecl *> &mockFuncs);
   
   static void serializeJsonTree(Json::Value &jsonRoot, const NameValueNode* node);
   
   
   std::unique_ptr<NameValueNode> _treeDataFromAST;
   std::unique_ptr<NameValueNode> _treeFromJson;

   
   const FunctionDeclKeySetMap& _funcDeclsMap;
   const FunctionDeclKeySetMap& _mockDeclsMap;
  
   static const unsigned int _defaultExampleContentSize;
   
};

#endif /*_UTBuilder_UnitTestData_h__*/