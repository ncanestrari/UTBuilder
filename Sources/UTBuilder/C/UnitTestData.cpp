#include "UnitTestData.h"

#include <clang/AST/Decl.h>


const unsigned int UnitTestData::_defaultContentSize = 2;


UnitTestData::UnitTestData(const FunctionDeclKeySetMap& funcDeclsMap,
                           const FunctionDeclKeySetMap& mockDeclsMap)
: _funcDeclsMap(funcDeclsMap)
, _mockDeclsMap(mockDeclsMap)
{
}

// UnitTestData::UnitTestData( const clang::FunctionDecl *funcDecl, const std::set<const clang::FunctionDecl *> &mockFuncs)
// {
//    if ( funcDecl != nullptr )
//    {
//       _tree = std::unique_ptr<NameValueNode>(buildTree(funcDecl, mockFuncs));
//    }
//    else {
//       // error management! 
//    }
//    
// }


UnitTestData::~UnitTestData()
{}


void UnitTestData::clear()
{
}

NameValueNode* UnitTestData::buildCollectionTree()
{
   _treeDataFromAST = std::unique_ptr<NameValueNode>( NameValueNode::createObject("dataAST") );
 
   /**
    *   root 
    *      {
    *      "desc" : {}   object
    *      "mocks" : []  array
    *      "funcs" : []  array
    *       }
    */
      
   NameValueNode* descNode = buildDescTree();
   _treeDataFromAST->addChild(descNode);
   
   NameValueNode* mocksNode = buildMocksTree();
   _treeDataFromAST->addChild(mocksNode);
   
   NameValueNode* funcsNode = buildFunctionsTree();
   _treeDataFromAST->addChild(funcsNode); 
   
}
   
NameValueNode* UnitTestData::buildDescTree()
{
   NameValueNode* descNode = NameValueNode::createObject("desc");
   
   
   return descNode;
}

NameValueNode* UnitTestData::buildMocksTree()
{
   NameValueNode* mocksNode = NameValueNode::createArray("mocks");
   
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
      
      NameValueNode* objectNode = NameValueNode::createObject(std::to_string(objectCounter).c_str());
      mocksNode->addChild(objectNode);
      objectCounter++;
      
      const clang::FunctionDecl *funcDecl = objIter.first;
      const std::string name = funcDecl->getNameAsString();   
      const std::set<const clang::FunctionDecl *> &mockDeclSet = objIter.second;
      
      auto* nameNode = TypeNameValueNode<const clang::FunctionDecl*>::create("_name", funcDecl, name.c_str());
      objectNode->addChild(nameNode);
      
      NameValueNode* contentObjNode = buildMockContentTree(funcDecl, mockDeclSet); //NameValueNode::createArrayNode("content");
      objectNode->addChild(contentObjNode);      
   }   
   
   return mocksNode;
}

NameValueNode* UnitTestData::buildFunctionsTree()
{
   NameValueNode* funcsNode = NameValueNode::createArray("funcs");
   
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
      
      NameValueNode* objectNode = NameValueNode::createObject(std::to_string(objectCounter).c_str());
      funcsNode->addChild(objectNode);
      objectCounter++;
      
      const clang::FunctionDecl *funcDecl = objIter.first;
      const std::string name = funcDecl->getNameAsString(); 
      const std::set<const clang::FunctionDecl *> &mockDeclSet = objIter.second;

      auto* nameNode = TypeNameValueNode<const clang::FunctionDecl*>::create("_name", funcDecl, name.c_str());
      objectNode->addChild(nameNode);
      
      NameValueNode* contentObjNode = buildFunctionContentTree(funcDecl, mockDeclSet); //NameValueNode::createArrayNode("content");
      objectNode->addChild(contentObjNode);      
   }   
   
   return funcsNode;
}


NameValueNode* UnitTestData::buildMockContentTree( const clang::FunctionDecl *funcDecl,
                                                   const std::set<const clang::FunctionDecl *> &mockFuncs)
{
   NameValueNode* contentNode = NameValueNode::createArray("content");
   
   /**
    * "content" : // array
    * [
    *           {// object 0 
    *               "input" : {},
    *               "mock-funcs-call" : {},
    *               "output" : 
    *               {
    *                  "..." : {}
    *                  "retval" : {}
    *               }
    *           }
    * ]
    */
      
   for (unsigned int  indexCounter=0; indexCounter<_defaultContentSize; ++indexCounter) {
      
      NameValueNode* contentElemArray = NameValueNode::createObject(std::to_string(indexCounter).c_str());
   
      if (funcDecl->getNumParams() > 0) {
         
         NameValueNode* inputNode = NameValueNode::createObject("input");
         
         for (const auto& field : funcDecl->params()) {
            
            auto node = TypeNameValueNode<clang::QualType>::create(field->getNameAsString().c_str(), field->getType(), "");
   //          root->addChild(field->getNameAsString().c_str(), field->getType(), "");
         }
         
         contentElemArray->addChild(inputNode);
      }
      
      
      NameValueNode* outputNode = NameValueNode::createObject("output");
      if (funcDecl->getNumParams() > 0) {
      
         for (const auto& field : funcDecl->params()) {
            auto node = TypeNameValueNode<clang::QualType>::create(field->getNameAsString().c_str(), field->getType(), "");
   //          root->addChild(field->getNameAsString().c_str(), field->getType(), "");
         }
      }
      
      auto node = TypeNameValueNode<clang::QualType>::create("retval", funcDecl->getReturnType(), "");
      outputNode->addChild(node);
      contentElemArray->addChild(outputNode);
      
      // add return type
   //    root->addChild("retval", funcDecl->getReturnType(), "");
      
      
      if ( mockFuncs.size() > 0 )
      {
         NameValueNode* mocksNode = NameValueNode::createObject("mock-funcs-call");
         
         std::string value;
         for (const clang::FunctionDecl* iter : mockFuncs) {
            value = iter->getNameAsString() + "_0";
            auto node = TypeNameValueNode<const clang::FunctionDecl*>::create(iter->getNameAsString().c_str(), iter, value.c_str());
   //          root->addChild(iter->getNameAsString().c_str(), iter, value.c_str());
         }
         
         contentElemArray->addChild(mocksNode);
      }
      
      contentNode->addChild(contentElemArray);
   }
   
   return contentNode;
}

NameValueNode* UnitTestData::buildFunctionContentTree( const clang::FunctionDecl *funcDecl,
                                                       const std::set<const clang::FunctionDecl *> &mockFuncs)
{
   NameValueNode* contentNode = NameValueNode::createArray("content");
   
   /**
    * "content" : // array
    * [
    *           {// object 0 
    *               "input" : {}, // object
    *               "mock-funcs-call" : {}, // object
    *               "output" :  // object
    *               {
    *                  "..." : {} // object
    *                  "retval" : {} // object
    *               }
    *           }
    * ]
    */
      
   for (unsigned int  indexCounter=0; indexCounter<_defaultContentSize; ++indexCounter) {
      
      NameValueNode* contentElemArray = NameValueNode::createObject(std::to_string(indexCounter).c_str());
      
      if (funcDecl->getNumParams() > 0) {
         
         NameValueNode* inputNode = NameValueNode::createObject("input");
         
         for (const auto& field : funcDecl->params()) {
            
            auto node = TypeNameValueNode<clang::QualType>::create(field->getNameAsString().c_str(), field->getType(), "");
   //          root->addChild(field->getNameAsString().c_str(), field->getType(), "");
         }
         
         contentElemArray->addChild(inputNode);
      }
      
      
      NameValueNode* outputNode = NameValueNode::createObject("output");
      if (funcDecl->getNumParams() > 0) {
      
         for (const auto& field : funcDecl->params()) {
            auto node = TypeNameValueNode<clang::QualType>::create(field->getNameAsString().c_str(), field->getType(), "");
   //          root->addChild(field->getNameAsString().c_str(), field->getType(), "");
         }
      }
      
      auto node = TypeNameValueNode<clang::QualType>::create("retval", funcDecl->getReturnType(), "");
      outputNode->addChild(node);
      contentElemArray->addChild(outputNode);
      
      // add return type
   //    root->addChild("retval", funcDecl->getReturnType(), "");
      
      
      if ( mockFuncs.size() > 0 )
      {
         NameValueNode* mocksNode = NameValueNode::createObject("mock-funcs-call");
         
         std::string value;
         for (const clang::FunctionDecl* iter : mockFuncs) {
            value = iter->getNameAsString() + "_0";
            auto node = TypeNameValueNode<const clang::FunctionDecl*>::create(iter->getNameAsString().c_str(), iter, value.c_str());
   //          root->addChild(iter->getNameAsString().c_str(), iter, value.c_str());
         }
         
         contentElemArray->addChild(mocksNode);
      }
   
      contentNode->addChild(contentElemArray);
   }
   
   return contentNode;
}



NameValueNode* UnitTestData::buildTreeFromAST( const clang::FunctionDecl *funcDecl,
                                    const std::set<const clang::FunctionDecl *> &mockFuncs)
{
}


void UnitTestData::serializeJson(Json::Value &jsonParent) const
{   

}

void UnitTestData::deSerializeJson(const Json::Value &jsonRoot, const void *) 
{   

}