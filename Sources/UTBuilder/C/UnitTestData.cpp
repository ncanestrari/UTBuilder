#include "UnitTestData.h"

#include <clang/AST/Decl.h>

#include <stack>

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
   NameValueNode* mocksArray = NameValueNode::createArray("mocks");
   
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
      
      NameValueNode* objectNode = NameValueNode::createArrayElement(std::to_string(objectCounter).c_str());
      mocksArray->addChild(objectNode);
      objectCounter++;
      
      const clang::FunctionDecl *funcDecl = objIter.first;
      const std::string name = funcDecl->getNameAsString();   
      const std::set<const clang::FunctionDecl *> &mockDeclSet = objIter.second;
      
      auto* nameNode = TypeNameValueNode<const clang::FunctionDecl*>::create("_name", funcDecl, name.c_str());
      objectNode->addChild(nameNode);
      
      NameValueNode* contentObjNode = buildMockContentTree(funcDecl, mockDeclSet); //NameValueNode::createArrayNode("content");
      objectNode->addChild(contentObjNode);      
   }   
   
   return mocksArray;
}

NameValueNode* UnitTestData::buildFunctionsTree()
{
   NameValueNode* funcsArray = NameValueNode::createArray("funcs");
   
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
      
      NameValueNode* objectNode = NameValueNode::createArrayElement(std::to_string(objectCounter).c_str());
      funcsArray->addChild(objectNode);
      objectCounter++;
      
      const clang::FunctionDecl *funcDecl = objIter.first;
      const std::string name = funcDecl->getNameAsString(); 
      const std::set<const clang::FunctionDecl *> &mockDeclSet = objIter.second;

      auto* nameNode = TypeNameValueNode<const clang::FunctionDecl*>::create("_name", funcDecl, name.c_str());
      objectNode->addChild(nameNode);
      
      NameValueNode* contentObjNode = buildFunctionContentTree(funcDecl, mockDeclSet); //NameValueNode::createArrayNode("content");
      objectNode->addChild(contentObjNode);      
   }   
   
   return funcsArray;
}


NameValueNode* UnitTestData::buildMockContentTree( const clang::FunctionDecl *funcDecl,
                                                   const std::set<const clang::FunctionDecl *> &mockFuncs)
{
   NameValueNode* contentArray = NameValueNode::createArray("content");
   
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
      
      NameValueNode* contentElemArray = NameValueNode::createArrayElement(std::to_string(indexCounter).c_str());
      contentArray->addChild(contentElemArray);
   
//       input
      if (funcDecl->getNumParams() > 0) {
         
         NameValueNode* inputNode = NameValueNode::createObject("input");
         contentElemArray->addChild(inputNode);
         
         for (const auto& field : funcDecl->params()) {
            
            auto* child = TypeNameValueNode<clang::QualType>::create(field->getNameAsString().c_str(), field->getType(), "");
            inputNode->addChild(child);
         }
         
         
      }
      
//       output
      NameValueNode* outputNode = NameValueNode::createObject("output");
      contentElemArray->addChild(outputNode);
      
      if (funcDecl->getNumParams() > 0) {
      
         for (const auto& field : funcDecl->params()) {
            auto* child = TypeNameValueNode<clang::QualType>::create(field->getNameAsString().c_str(), field->getType(), "");
            outputNode->addChild(child);
         }
      }
      
      auto* retvalNode = TypeNameValueNode<clang::QualType>::create("retval", funcDecl->getReturnType(), "");
      outputNode->addChild(retvalNode);
      
//       mock-funcs-call
      if ( mockFuncs.size() > 0 )
      {
         NameValueNode* mocksNode = NameValueNode::createObject("mock-funcs-call");
         contentElemArray->addChild(mocksNode);
         
         std::string value;
         for (const clang::FunctionDecl* iter : mockFuncs) {
            value = iter->getNameAsString() + "_" + std::to_string(indexCounter);
            auto* child = TypeNameValueNode<const clang::FunctionDecl*>::create(iter->getNameAsString().c_str(), iter, value.c_str());
            mocksNode->addChild(child);
         }
         
      }
      
   }
   
   return contentArray;
}

NameValueNode* UnitTestData::buildFunctionContentTree( const clang::FunctionDecl *funcDecl,
                                                       const std::set<const clang::FunctionDecl *> &mockFuncs)
{
   NameValueNode* contentArray = NameValueNode::createArray("content");
   
   /**
    * "content" : // array
    * [
    *           {// elementArray 0 
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
      
      NameValueNode* contentElemArray = NameValueNode::createArrayElement(std::to_string(indexCounter).c_str());
      contentArray->addChild(contentElemArray);
      
//       input
      if (funcDecl->getNumParams() > 0) {
         
         NameValueNode* inputNode = NameValueNode::createObject("input");
         contentElemArray->addChild(inputNode);
         
         for (const auto& field : funcDecl->params()) {
            
            auto* child = TypeNameValueNode<clang::QualType>::create(field->getNameAsString().c_str(), field->getType(), "");
            inputNode->addChild(child);
         }
         
         
      }
      
//       output
      NameValueNode* outputNode = NameValueNode::createObject("output");
      contentElemArray->addChild(outputNode);
      
      if (funcDecl->getNumParams() > 0) {
      
         for (const auto& field : funcDecl->params()) {
            auto* child = TypeNameValueNode<clang::QualType>::create(field->getNameAsString().c_str(), field->getType(), "");
            outputNode->addChild(child);
         }
      }
      
      auto* retvalNode = TypeNameValueNode<clang::QualType>::create("retval", funcDecl->getReturnType(), "");
      outputNode->addChild(retvalNode);
      
//       mock-funcs-call
      if ( mockFuncs.size() > 0 )
      {
         NameValueNode* mocksNode = NameValueNode::createObject("mock-funcs-call");
         contentElemArray->addChild(mocksNode);
         
         std::string value;
         for (const clang::FunctionDecl* iter : mockFuncs) {
            value = iter->getNameAsString() + "_" + std::to_string(indexCounter);
            auto* child = TypeNameValueNode<const clang::FunctionDecl*>::create(iter->getNameAsString().c_str(), iter, value.c_str());
            mocksNode->addChild(child);
         }
         
      }
   
   }
   
   return contentArray;
}



NameValueNode* UnitTestData::buildTreeFromAST( const clang::FunctionDecl *funcDecl,
                                    const std::set<const clang::FunctionDecl *> &mockFuncs)
{
}


static void addJsonArrayComment(Json::Value& jsonValue, const NameValueNode* node)
{
}

static void addJsonArrayElementComment(Json::Value& jsonValue, const NameValueNode* node)
{
   std::ostringstream comment;
   const int index = node->getIndex();
   comment <<  "// \"_name\"_test_" << index;
   jsonValue.setComment(comment.str().c_str(), comment.str().length(), Json::commentBefore);
}

static void addJsonObjectComment(Json::Value& jsonValue, const NameValueNode* node)
{
   std::ostringstream comment;
   
   if ( dynamic_cast<const TypeNameValueNode<clang::QualType>* >(node) ) {
   
      const clang::QualType qualType = *static_cast<clang::QualType*>(node->getType());
      if (qualType.isNull() == false) {
         comment << "// struct " << qualType.getAsString();
         jsonValue.setComment(comment.str().c_str(), comment.str().length(), Json::commentAfterOnSameLine);
      }
   }  
}

static void addJsonValueComment(Json::Value& jsonValue, const NameValueNode* node)
{
   if ( dynamic_cast<const TypeNameValueNode<clang::QualType>* >(node) ) {
   
      std::string comment;
      const clang::QualType qualType = *static_cast<clang::QualType*>(node->getType());
      if (qualType.isNull() == false) {
         const clang::QualType canonicalQualType = qualType->getCanonicalTypeInternal();
         comment = "// type: " + qualType.getAsString() + " (" + canonicalQualType.getAsString() + ") ";
         jsonValue.setComment(comment.c_str(), comment.length(), Json::commentAfterOnSameLine);            
      }
   }
}

static void addJsonStructValueComment(Json::Value& jsonValue, const NameValueNode* node)
{
   if ( dynamic_cast<const TypeNameValueNode<clang::QualType>* >(node) ) {
   
      std::string comment;
      const clang::QualType qualType = *static_cast<clang::QualType*>(node->getType());
      if (qualType.isNull() == false) {
         const clang::QualType canonicalQualType = qualType->getCanonicalTypeInternal();
         comment = "// struct " + qualType.getAsString();
         jsonValue.setComment(comment.c_str(), comment.length(), Json::commentAfterOnSameLine);            
      }
   }
}


void UnitTestData::serializeJsonTree(Json::Value& jsonParent, const NameValueNode* parentNode)
{

   const std::map< std::string, std::unique_ptr<NameValueNode> >& children = parentNode->getChildren();
   
   for (const auto& child : children) {
      
      const std::string& name = child.first;
      const NameValueNode* childNode = child.second.get();
      
      if (childNode->isObject() ) {
         
         jsonParent[name] = Json::Value(Json::objectValue);
         addJsonObjectComment(jsonParent[name], childNode);
         serializeJsonTree(jsonParent[name], childNode );          
      }
      else if (childNode->isArray() ) {
         
         jsonParent[name] = Json::Value(Json::arrayValue);
         addJsonArrayComment(jsonParent[name], childNode);
         serializeJsonTree(jsonParent[name], childNode );
      }
      else if (childNode->isArrayElement() ) {
         
         const int index = childNode->getIndex();
         jsonParent[index] = Json::Value(Json::objectValue);         
         addJsonArrayElementComment(jsonParent[index], childNode);
         serializeJsonTree(jsonParent[index], childNode );
      }
      else { 
         
         // if childNode is a struct {} keep recursing            
         if ( childNode->getNumChildern() > 0 ) {
            
            jsonParent[name] = Json::Value(Json::objectValue);
            addJsonStructValueComment(jsonParent[name], childNode); 
            serializeJsonTree(jsonParent[name], childNode);
         }
         // childNode is not a struct {} stop recursing  
         else {   
            jsonParent[name] = childNode->getValue();
            addJsonValueComment(jsonParent[name], childNode);             
         }
         
      }
   }
}

void UnitTestData::serializeJson(Json::Value &jsonRoot) const
{   
   jsonRoot = Json::Value(Json::objectValue);
   
   
//    UnitTestData::serializeJsonTree(jsonRoot, _treeDataFromAST.get() );

   typedef std::pair<Json::Value&, const NameValueNode*> JsonTreeNodePair;
   std::stack< JsonTreeNodePair > treeStack;
   
   treeStack.push( JsonTreeNodePair(jsonRoot, _treeDataFromAST.get()) );
   
   while ( !treeStack.empty() ) {
    
//     check if it's safe get a const reference of the top of the stack and call pop()
      const JsonTreeNodePair& topPair = treeStack.top();
      treeStack.pop();
      
      Json::Value& currentJson= topPair.first;
      const NameValueNode* currentNode = topPair.second;
      
      const std::map< std::string, std::unique_ptr<NameValueNode> >& children = currentNode->getChildren();
   
      for (const auto& child : children) {
         
         const std::string& name = child.first;
         const NameValueNode* childNode = child.second.get();
         
         if (childNode->isObject() ) {
            
            currentJson[name] = Json::Value(Json::objectValue);
            addJsonObjectComment(currentJson[name], childNode);
            
            treeStack.push( JsonTreeNodePair(currentJson[name], childNode) );         
         }
         else if (childNode->isArray() ) {
            
            currentJson[name] = Json::Value(Json::arrayValue);
            addJsonArrayComment(currentJson[name], childNode);
            
            treeStack.push( JsonTreeNodePair(currentJson[name], childNode) ); 
         }
         else if (childNode->isArrayElement() ) {
            
            const int index = childNode->getIndex();
            currentJson[index] = Json::Value(Json::objectValue);         
            addJsonArrayElementComment(currentJson[index], childNode);
   
            treeStack.push( JsonTreeNodePair(currentJson[index], childNode) ); 
         }
         else { 
            
            // if childNode is a struct {} keep recursing            
            if ( childNode->getNumChildern() > 0 ) {
               
               currentJson[name] = Json::Value(Json::objectValue);
               addJsonStructValueComment(currentJson[name], childNode); 
               
               treeStack.push( JsonTreeNodePair(currentJson[name], childNode) ); 
            }
            // childNode is not a struct {} stop recursing  
            else {   
               currentJson[name] = childNode->getValue();
               addJsonValueComment(currentJson[name], childNode);             
            }
            
         }
      }
      
   } 
}

void UnitTestData::deSerializeJson(const Json::Value &jsonRoot, const void *) 
{   

}