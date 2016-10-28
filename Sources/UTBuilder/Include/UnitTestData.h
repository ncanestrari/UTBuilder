#ifndef _UTBuilder_UnitTestData_h__
#define _UTBuilder_UnitTestData_h__

// #include "Serializable.h"
// #include "singleton.h"
#include "NameValueTypeNode.h"
#include "Results.h"

#include <set>

namespace clang{
   class FunctionDecl;
}


class UnitTestData //:  public Serializable //, public Singleton<UnitTestData>
{

public:
   
//    UnitTestData( const clang::FunctionDecl *funcDecl,
//              const std::set<const clang::FunctionDecl *> &mockFuncs = std::set<const clang::FunctionDecl *>());
   
   UnitTestData(const FunctionDeclKeySetMap   &funcDeclsMap,
                const FunctionDeclKeySetMap   &mockDeclsMap);
   
   UnitTestData(const UnitTestData&) = delete;
   
   ~UnitTestData();


//    Serializable interface
   void serializeJson(Json::Value &jsonRoot, const NameValueNode* data = nullptr ) const;
   void deSerializeJson(const Json::Value &jsonRoot );

   void serializeJsonData(Json::Value &jsonRoot) const;
   
   NameValueNode* buildCollectionTree();

   const NameValueNode* getTreeData() const { return _treeData.get(); }
   
   const NameValueNode* getDescData() const { return _treeData->getChild("desc"); }
   const NameValueNode* getFuncsData() const { return _treeData->getChild("funcs"); }
   const NameValueNode* getMocksData() const { return _treeData->getChild("mocks"); }
   
   const FunctionDeclKeySetMap& getFunctionToTest() const { return _funcDeclsMap; }
   const FunctionDeclKeySetMap& getFunctionToMock() const { return _mockDeclsMap; }
   
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
   
   
   
   NameValueNode* buildTreeFromAST( const clang::FunctionDecl *funcDecl,
                                    const std::set<const clang::FunctionDecl *> &mockFuncs);
   
   static void serializeJsonTree(Json::Value &jsonRoot, const NameValueNode* node);
   
   bool validiteData(void);
   
   NameValueNode* createValidatedNode(const NameValueNode* refQualTypeNode,const NameValueNode* jsonNode );
   
   
   std::unique_ptr<NameValueNode> _treeFromAST;
   std::unique_ptr<NameValueNode> _treeFromJson;
   std::unique_ptr<NameValueNode> _treeData;
   
   
   const FunctionDeclKeySetMap& _funcDeclsMap;
   const FunctionDeclKeySetMap& _mockDeclsMap;
  
   static const unsigned int _defaultExampleContentSize;
   
};

#endif /*_UTBuilder_UnitTestData_h__*/