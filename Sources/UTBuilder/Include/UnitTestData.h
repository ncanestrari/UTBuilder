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

   
   NameValueNode* buildCollectionTree();

   const NameValueNode* getTreeData() const { return _treeData.get(); }
   
   const NameValueNode* getDescData() const { return _treeData->getChild("desc"); }
   const NameValueNode* getFuncsData() const { return _treeData->getChild("funcs"); }
   const NameValueNode* getMocksData() const { return _treeData->getChild("mocks"); }
   
   const FunctionDeclKeySetMap& getFunctionToTest() const { return _funcDeclsMap; }
   const FunctionDeclKeySetMap& getFunctionToMock() const { return _mockDeclsMap; }
   
private:

   void clear(void);
   
   // temp
   void testActions() const;
   
   
   NameValueNode* buildDescTree();
   NameValueNode* buildTree(const char* treeName, const FunctionDeclKeySetMap& declMap );
   NameValueNode* buildContentTree( const clang::FunctionDecl *funcDecl,
                                            const std::set<const clang::FunctionDecl *>& funcs );

   
   
   bool buildValidateData(const NameValueNode* tempTreeFromJson);
   
   NameValueNode* createValidatedNode(const NameValueNode* refQualTypeNode,const NameValueNode* jsonNode );
   
   
   std::unique_ptr<NameValueNode> _treeFromAST;
   std::unique_ptr<NameValueNode> _treeData;
   
   
   const FunctionDeclKeySetMap& _funcDeclsMap;
   const FunctionDeclKeySetMap& _mockDeclsMap;
  
   static const unsigned int _defaultExampleContentSize;
   
};

#endif /*_UTBuilder_UnitTestData_h__*/