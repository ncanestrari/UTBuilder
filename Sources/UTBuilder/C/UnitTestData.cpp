#include "UnitTestData.h"

#include <clang/AST/Decl.h>

#include <stack>

#include "UnitTestDataUtils.h"

const unsigned int UnitTestData::_defaultExampleContentSize = 2;



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
      
      NameValueNode* objectNode = NameValueNode::createArrayElement( objectCounter );
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
      
      NameValueNode* objectNode = NameValueNode::createArrayElement(objectCounter);
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
      
   for (unsigned int  indexCounter=0; indexCounter<_defaultExampleContentSize; ++indexCounter) {
      
      NameValueNode* contentElemArray = NameValueNode::createArrayElement(indexCounter);
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
            value = iter->getNameAsString() + "_" + NameValueNode::_arrayIndex[indexCounter];
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
      
   for (unsigned int  indexCounter=0; indexCounter<_defaultExampleContentSize; ++indexCounter) {
      
      NameValueNode* contentElemArray = NameValueNode::createArrayElement(indexCounter);
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
            value = iter->getNameAsString() + "_" + NameValueNode::_arrayIndex[indexCounter];
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



void UnitTestData::serializeJsonTree(Json::Value& jsonParent, const NameValueNode* parentNode)
{

   const std::map< std::string, std::unique_ptr<NameValueNode> >& children = parentNode->getChildren();
   
   for (const auto& child : children) {
      
      const std::string& name = child.first;
      const NameValueNode* childNode = child.second.get();
      
      if (childNode->isObject() ) {
         
         jsonParent[name] = Json::Value(Json::objectValue);
         UnitTestDataUtils::addJsonObjectComment(jsonParent[name], childNode);
         serializeJsonTree(jsonParent[name], childNode );          
      }
      else if (childNode->isArray() ) {
         
         jsonParent[name] = Json::Value(Json::arrayValue);
         UnitTestDataUtils::addJsonArrayComment(jsonParent[name], childNode);
         serializeJsonTree(jsonParent[name], childNode );
      }
      else if (childNode->isArrayElement() ) {
         
         const int index = childNode->getIndex();
         jsonParent[index] = Json::Value(Json::objectValue);         
         UnitTestDataUtils::addJsonArrayElementComment(jsonParent[index], childNode);
         serializeJsonTree(jsonParent[index], childNode );
      }
      else { 
         
         // if childNode is a struct {} keep recursing            
         if ( childNode->getNumChildern() > 0 ) {
            
            jsonParent[name] = Json::Value(Json::objectValue);
            UnitTestDataUtils::addJsonStructValueComment(jsonParent[name], childNode); 
            serializeJsonTree(jsonParent[name], childNode);
         }
         // childNode is not a struct {} stop recursing  
         else {   
            jsonParent[name] = childNode->getValue();
            UnitTestDataUtils::addJsonValueComment(jsonParent[name], childNode);             
         }
         
      }
   }
}

void UnitTestData::serializeJson(Json::Value &jsonRoot) const
{   
   jsonRoot = Json::Value(Json::objectValue);
   
   
//    UnitTestData::serializeJsonTree(jsonRoot, _treeDataFromAST.get() );

   typedef std::pair<Json::Value&, const NameValueNode*> JsonTreeNodePair;
   std::stack< JsonTreeNodePair > Stack;
   
   Stack.push( JsonTreeNodePair(jsonRoot, _treeDataFromAST.get()) );
   
   while ( !Stack.empty() ) {
    
//     check if it's safe get a const reference of the top of the stack and call pop()
      const JsonTreeNodePair& topPair = Stack.top();
      Stack.pop();
      
      Json::Value& currentJson= topPair.first;
      const NameValueNode* currentNode = topPair.second;
      
      const std::map< std::string, std::unique_ptr<NameValueNode> >& children = currentNode->getChildren();
   
      for (const auto& child : children) {
         
         const std::string& name = child.first;
         const NameValueNode* childNode = child.second.get();
         
         if (childNode->isObject() ) {
            
            currentJson[name] = Json::Value(Json::objectValue);
            UnitTestDataUtils::addJsonObjectComment(currentJson[name], childNode);
            
            Stack.push( JsonTreeNodePair(currentJson[name], childNode) );         
         }
         else if (childNode->isArray() ) {
            
            currentJson[name] = Json::Value(Json::arrayValue);
            UnitTestDataUtils::addJsonArrayComment(currentJson[name], childNode);
            
            Stack.push( JsonTreeNodePair(currentJson[name], childNode) ); 
         }
         else if (childNode->isArrayElement() ) {
            
            const int index = childNode->getIndex();
            currentJson[index] = Json::Value(Json::objectValue);         
            UnitTestDataUtils::addJsonArrayElementComment(currentJson[index], childNode);
   
            Stack.push( JsonTreeNodePair(currentJson[index], childNode) ); 
         }
         else { 
            
            // if childNode is a struct {} keep recursing            
            if ( childNode->getNumChildern() > 0 ) {
               
               currentJson[name] = Json::Value(Json::objectValue);
               UnitTestDataUtils::addJsonStructValueComment(currentJson[name], childNode); 
               
               Stack.push( JsonTreeNodePair(currentJson[name], childNode) ); 
            }
            // childNode is not a struct {} stop recursing  
            else {   
               currentJson[name] = childNode->getValue();
               UnitTestDataUtils::addJsonValueComment(currentJson[name], childNode);             
            }
            
         }
      }
      
   } 
}


static NameValueNode* deSerializeMocksContent(const Json::Value &jsonContent, const NameValueNode* contentRefTree)
{
   NameValueNode* contentArray = NameValueNode::createArray("content");
   
   const unsigned int size = jsonContent.size();
//    _funcDecl = funcTestContentfromAST->getFunctionDecl();
   const NameValueNode* elemContentRefTree = contentRefTree->getChildren().begin()->second.get();
   
   for (unsigned int i = 0; i < size; ++i) {
      
       const Json::Value& jsonTest = jsonContent[i];       
       
       NameValueNode* contentArrayElem = NameValueNode::createArrayElement(i);
       contentArray->addChild(contentArrayElem);
       
/*       
       test->deSerializeJson( jsonTest, funcTestReference.get() );
       _tests[i] = test;

       
      NameValueNode* contentElemArray = NameValueNode::createArrayElement(indexCounter);
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
            value = iter->getNameAsString() + "_" + NameValueNode::_arrayIndex[indexCounter];
            auto* child = TypeNameValueNode<const clang::FunctionDecl*>::create(iter->getNameAsString().c_str(), iter, value.c_str());
            mocksNode->addChild(child);
         }
         
      }*/
      
   }
   
   return contentArray;
}



static void deSerializeMocks(const Json::Value &jsonRoot, NameValueNode* data, const NameValueNode* refTree)
{
    //load json defined mocks
   const Json::Value& mocksRoot = jsonRoot["mocks"];
   if( mocksRoot.empty() ){
      std::cout << "functions to mock not found in input json file: " << std::endl;
   }
   
   NameValueNode* mocksArray = NameValueNode::createArray("mocks");
   data->addChild(mocksArray);
 
   const NameValueNode* mocksRefParent = refTree->getChild("mocks");

   const unsigned int size = mocksRoot.size();

   size_t objectCounter = 0;
   for (unsigned int i = 0; i < size; ++i) {
      
      const Json::Value& value = mocksRoot[i];
      const Json::Value funcNameValue = value.get("_name", "");
      const std::string funcName = funcNameValue.asString();

      // check validity
      const NameValueNode* refFuncContent = UnitTestDataUtils::findContentFromAST(funcName, mocksRefParent);
         
      if ( refFuncContent != nullptr) {
//          the func defined exists
         
         NameValueNode* arrElementNode = NameValueNode::createArrayElement( objectCounter );
         mocksArray->addChild(arrElementNode);
         objectCounter++;
      
         NameValueNode* nameNode = NameValueNode::createValue("_name", funcName.c_str() );
         arrElementNode->addChild(nameNode);
         
         const Json::Value contentValue = value.get("content", "");
         if ( contentValue.empty() )
            continue;
         
         NameValueNode* contentObjNode = deSerializeMocksContent(contentValue, refFuncContent->getChild("content") ); //NameValueNode::createArrayNode("content");
         arrElementNode->addChild(contentObjNode); 
         
         
//          auto* nameNode = TypeNameValueNode<const clang::FunctionDecl*>::create("_name", funcDecl, name.c_str());
//          arrElementNode->addChild(nameNode);
         
//          NameValueNode* contentObjNode = deSerializeMocksContent(funcDecl, mockDeclSet); //NameValueNode::createArrayNode("content");
//          arrElementNode->addChild(contentObjNode);   
         
      }
      else {
//          the func defined doesn't exist
         
      }
      
   }
}

static void deSerializeFuncs(const Json::Value &jsonRoot, NameValueNode* data, const NameValueNode* refTree)
{
    //load json defined unit/module tests
   const Json::Value& funcsRoot = jsonRoot["funcs"];
   if( funcsRoot.empty() ){
      std::cout << "functions to test not found in input json file: " << std::endl;
   }
   
   NameValueNode* funcsNode = NameValueNode::createArray("funcs");
   data->addChild(funcsNode);
   
}


void UnitTestData::deSerializeJson(const Json::Value &jsonRoot, const void *) 
{   
   
   _treeFromJson = std::unique_ptr<NameValueNode>( NameValueNode::createObject("dataJson") );
   
   deSerializeMocks(jsonRoot, _treeFromJson.get(), _treeDataFromAST.get() );
   deSerializeFuncs(jsonRoot, _treeFromJson.get(), _treeDataFromAST.get() );
   
}