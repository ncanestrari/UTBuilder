#ifndef _UTBuilder_UnitTestData_h__
#define _UTBuilder_UnitTestData_h__


#include "TreeNodes.h"
#include "ASTinfo.h"

#include <json/json.h>
#include <set>

namespace clang{
   class FunctionDecl;
}

class NameValueNode;


class UnitTestData
{

public:
   
   
   UnitTestData(const ASTinfo& _info);
   
   UnitTestData(const UnitTestData&) = delete;
   
   ~UnitTestData();


//    Serializable interface
   void serializeJson(Json::Value &jsonRoot, const NameValueNode* data = nullptr ) const;
   void deSerializeJson(const Json::Value &jsonRoot );

   
   NameValueNode* buildTreeFromAST();

   const NameValueNode* getTreeData() const { return _treeData.get(); }
   
   const NameValueNode* getDescData() const { return _treeData->getChild("desc"); }
   const NameValueNode* getFuncsData() const { return _treeData->getChild("funcs"); }
   const NameValueNode* getMocksData() const { return _treeData->getChild("mocks"); }
   
   const FunctionDeclKeySetMap* getFunctionToTest() const { return _funcDeclsMap; }
   const FunctionDeclKeySetMap* getFunctionToMock() const { return _mockDeclsMap; }
   
private:

   void clear(void);
   
   // temp
   void testActions() const;
   
   
   NameValueNode* buildDescTree();
   NameValueNode* buildTree(const char* treeName, const FunctionDeclKeySetMap& declMap );
   NameValueNode* buildContentTree( const clang::FunctionDecl *funcDecl,
                                            const std::set<const clang::FunctionDecl *>& funcs );

   
   bool buildTreeData(const NameValueNode* tempTreeFromJson);
   
   NameValueNode* createValidatedNode(const NameValueNode* refQualTypeNode,const NameValueNode* jsonNode );
   
   
   std::unique_ptr<NameValueNode> _treeFromAST;
   std::unique_ptr<NameValueNode> _treeData;
   
   
   // store these const references from ASTinfo
   const FunctionDeclKeySetMap* _funcDeclsMap;
   const FunctionDeclKeySetMap* _mockDeclsMap;
  
   
   static const unsigned int _defaultExampleContentSize;
   
};

#endif /*_UTBuilder_UnitTestData_h__*/