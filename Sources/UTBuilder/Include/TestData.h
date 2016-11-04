#ifndef _UTBuilder_TestData_h__
#define _UTBuilder_TestData_h__

// #include "Serializable.h"
// #include "singleton.h"
#include "TreeNode.h"
#include "Results.h"

#include <set>

namespace clang{
   class FunctionDecl;
}


class TestData //:  public Serializable //, public Singleton<UnitTestData>
{

public:
   
   TestData( const FunctionDeclKeySetMap   &funcDeclsMap,
             const FunctionDeclKeySetMap   &mockDeclsMap);
   
   TestData(const TestData&) = delete;
   
   ~TestData();


//    Serializable interface
   void serializeJson(Json::Value &jsonRoot, const Node* data = nullptr ) const;
   void deSerializeJson(const Json::Value &jsonRoot );

   Node* buildTreeFromAST();
   

   const Node* getTreeData() const { return _treeData.get(); }
   
   const Node* getDescData() const { return dynamic_cast<const Node*>(_treeData->getChild("desc")); }
   const Node* getFuncsData() const { return dynamic_cast<const Node*>(_treeData->getChild("funcs")); }
   const Node* getMocksData() const { return dynamic_cast<const Node*>(_treeData->getChild("mocks")); }
   
   const FunctionDeclKeySetMap& getFunctionToTest() const { return _funcDeclsMap; }
   const FunctionDeclKeySetMap& getFunctionToMock() const { return _mockDeclsMap; }
   
private:

   void clear(void);
   
   Node* buildDescTree();
   Node* buildMocksTree();
   Node* buildFunctionsTree();
   
   std::unique_ptr<Node> _treeFromAST;
   std::unique_ptr<Node> _treeFromJson;
   std::unique_ptr<Node> _treeData;
   
   
   const FunctionDeclKeySetMap& _funcDeclsMap;
   const FunctionDeclKeySetMap& _mockDeclsMap;
  
   static const unsigned int _defaultExampleContentSize;
   
};

#endif /*_UTBuilder_TestData_h__*/