#include "TestData.h"

#include <clang/AST/Decl.h>


TestData::TestData(const FunctionDeclKeySetMap& funcDeclsMap,
                           const FunctionDeclKeySetMap& mockDeclsMap)
: _funcDeclsMap(funcDeclsMap)
, _mockDeclsMap(mockDeclsMap)
{
}


TestData::~TestData()
{}


void TestData::clear()
{
}


Node* TestData::buildTreeFromAST()
{
   _treeFromAST = std::unique_ptr<Node>( Node::createObject("dataAST") );
 
   /**
    *   root 
    *      {
    *      "desc" : {}   object
    *      "mocks" : []  array
    *      "funcs" : []  array
    *       }
    */
      
   Node* descNode = buildDescTree();
   _treeFromAST->addChild(descNode);
   
//    NameValueNode* mocksNode = buildMocksTree();
//    _treeFromAST->addChild(mocksNode);
//    
//    NameValueNode* funcsNode = buildFunctionsTree();
//    _treeFromAST->addChild(funcsNode); 
   
}

Node* TestData::buildDescTree()
{
   Node* descNode = Node::createObject("desc");
   
//    files : array
   Node* filesChildNode = Node::createArray("files");
   descNode->addChild(filesChildNode);
      
//    dirs : array
   Node* dirsChildNode = Node::createArray("dirs");
   descNode->addChild(dirsChildNode);
   
//    output : value
   Node* outputChildNode = Node::createValue("output");
   descNode->addChild(outputChildNode);            
                
   
   return descNode;
}


Node* TestData::buildMocksTree()
{
   Node* mocksArray = Node::createArray("mocks");
   
   /**
    *    "mocks" : // array
    *   [
    *      { //object 0
    *         "_name" : "mock function name 1", // value
    *         "content" : []                    // array
    *      },
    *      { // object 1
    *         "_name" : "mock function name 2",  // value
    *         "content" : []                     // array
    *      }
    *   ],
    */
   
   size_t objectCounter = 0;
   for (const auto& objIter : _mockDeclsMap) {
      
      Node* objectNode = Node::createArrayElement( objectCounter );
      mocksArray->addChild(objectNode);
      objectCounter++;
      
      const clang::FunctionDecl *funcDecl = objIter.first;
      const std::string name = funcDecl->getNameAsString();   
      const std::set<const clang::FunctionDecl *> &mockDeclSet = objIter.second;
      
//       FunctionDeclNode* nameNode = FunctionDeclNode::create("_name", funcDecl, name.c_str());
//       objectNode->addChild(nameNode->getSuperClass());
      
//       NameValueNode* contentObjNode = buildMockContentTree(funcDecl, mockDeclSet); //NameValueNode::createArrayNode("content");
//       NameValueNode* contentObjNode = buildContentTree(funcDecl, mockDeclSet);
//       objectNode->addChild(contentObjNode);      
   }   
   
   return mocksArray;
}

Node* TestData::buildFunctionsTree()
{
   Node* funcsArray = Node::createArray("funcs");
   
   /**
    * "funcs" : // array
    * [
    *      {    // object 0
    *         "_name" : "function to test name 1",      // value
    *         "content" : []                            // array
    *      },
    *     {     // object 1
    *         "_name" : "function to test name 2",      // value
    *         "content" : []                            // array
    *      }
    * ],
    */
   
   size_t objectCounter = 0;
   for (const auto& objIter : _funcDeclsMap) {
      
      Node* objectNode = Node::createArrayElement(objectCounter);
      funcsArray->addChild(objectNode);
      objectCounter++;
      
      const clang::FunctionDecl *funcDecl = objIter.first;
      const std::string name = funcDecl->getNameAsString(); 
      const std::set<const clang::FunctionDecl *> &mockDeclSet = objIter.second;

//       FunctionDecl* nameNode = FunctionDeclNode::create("_name", funcDecl, name.c_str());
//       objectNode->addChild(nameNode);
      
//       NameValueNode* contentObjNode = buildFunctionContentTree(funcDecl, mockDeclSet); //NameValueNode::createArrayNode("content");
//       Node* contentObjNode = buildContentTree(funcDecl, mockDeclSet);
//       objectNode->addChild(contentObjNode);      
   }   
   
   return funcsArray;
}

void TestData::serializeJson(Json::Value &jsonRoot, const Node* data) const
{   
   jsonRoot = Json::Value(Json::objectValue);
   
}


void TestData::deSerializeJson(const Json::Value &jsonRoot ) 
{   
      

   
}


