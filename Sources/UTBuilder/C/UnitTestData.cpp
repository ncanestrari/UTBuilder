#include "UnitTestData.h"

#include <clang/AST/Decl.h>

#include <stack>

#include <stdexcept>
using std::logic_error;

#include "TreeNodes.h"
#include "UnitTestDataUtils.h"

#include "ClangCompiler.h"


// #include "NameValueTypeActions.h"
// #include "NameValueTypeVisitor.h"

const unsigned int UnitTestData::_defaultExampleContentSize = 2;



UnitTestData::UnitTestData(const ClangCompiler* compiler)
: _allTypesMap(&compiler->getASTinfo().getAllTypesMap() )
, _funcDeclsMap(&compiler->getASTinfo().getFunctionsToUnitTestMap() )
, _mockDeclsMap(&compiler->getASTinfo().getFunctionsToMockMap() )
, _descFileNames( compiler->getProjectDescription().getAllFileNames() )
, _outputFileName( compiler->getProjectDescription().getOutputFileName() ) 
{
//    build the _treeFromAST
   buildTreeFromAST();
}
 
 
UnitTestData::~UnitTestData() = default;


void UnitTestData::clear()
{
}


NameValueNode* UnitTestData::buildTreeFromAST()
{
   _treeFromAST = std::unique_ptr<NameValueNode>( NameValueNode::createObject("dataAST") );
 
   /**
    *   root 
    *      {
    *      "desc" : {}   object
    *      "mocks" : []  array
    *      "funcs" : []  array
    *       }
    */
      
   NameValueNode* descNode = buildDescTree();
   _treeFromAST->addChild(descNode);
   
   NameValueNode* mocksNode = buildTree("mocks", *_mockDeclsMap);
   _treeFromAST->addChild(mocksNode);
   
   NameValueNode* funcsNode = buildTree("funcs", *_funcDeclsMap);
   _treeFromAST->addChild(funcsNode); 
   
   NameValueNode* globalsNode = buildGlobalsTree();
   _treeFromAST->addChild(globalsNode);
   
}
   
NameValueNode* UnitTestData::buildDescTree()
{
   NameValueNode* descNode = NameValueNode::createObject("desc");
   
//    files
   NameValueNode* filesChildNode = NameValueNode::createArray("files");
   descNode->addChild(filesChildNode);
   
   unsigned int counter = 0;
   for ( const auto& file : _descFileNames ) {
      NameValueNode* childNode = NameValueNode::createArrayElement( counter, file.c_str());
      filesChildNode->addChild(childNode);
      counter++;
   }
      
//    dirs
   NameValueNode* dirsChildNode = NameValueNode::createArray("dirs");
   descNode->addChild(dirsChildNode);
   
//    output
   NameValueNode* outputChildNode = NameValueNode::createValue("output", _outputFileName.c_str());
   descNode->addChild(outputChildNode);            
       
 
   return descNode;
}

NameValueNode* UnitTestData::buildGlobalsTree()
{
   NameValueNode* descNode = NameValueNode::createArray("globals");
}

NameValueNode* UnitTestData::buildTree(const char* treeName, const FunctionDeclKeySetMap& declMap )
{
  NameValueNode* arrayNode = NameValueNode::createArray(treeName);
   
   
   size_t objectCounter = 0;
   for (const auto& objIter : declMap) {
      
      NameValueNode* objectNode = NameValueNode::createArrayElement( objectCounter );
      arrayNode->addChild(objectNode);
      objectCounter++;
      
      const clang::FunctionDecl *funcDecl = objIter.first;
      const std::string name = funcDecl->getNameAsString();   
      const std::set<const clang::FunctionDecl *> &mockDeclSet = objIter.second;
      
      auto* nameNode = FunctionDeclNode::create("_name", funcDecl, name.c_str());
      objectNode->addChild(nameNode);
      
      NameValueNode* contentObjNode = buildContentTree(funcDecl, mockDeclSet);
      objectNode->addChild(contentObjNode);      
   }   
   
   return arrayNode;
}


NameValueNode* UnitTestData::buildContentTree(const clang::FunctionDecl*                   funcDecl,
                                              const std::set<const clang::FunctionDecl *>& funcs)
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
            
            auto* child = QualTypeNode::create(field->getNameAsString().c_str(), field->getType(), "");
            inputNode->addChild(child);
         }
         
         
      }
      
//       output
      NameValueNode* outputNode = NameValueNode::createObject("output");
      contentElemArray->addChild(outputNode);
      
      if (funcDecl->getNumParams() > 0) {
      
         for (const auto& field : funcDecl->params()) {
            auto* child = QualTypeNode::create(field->getNameAsString().c_str(), field->getType(), "");
            outputNode->addChild(child);
         }
      }
      
      auto* retvalNode = QualTypeNode::create("retval", funcDecl->getReturnType(), "");
      outputNode->addChild(retvalNode);
      
//       mock-funcs-call
      NameValueNode* mocksNode = NameValueNode::createObject("mock-funcs-call");
      contentElemArray->addChild(mocksNode);
	 
      if ( funcs.size() > 0 ){         
         std::string value;
         for (const clang::FunctionDecl* iter : funcs) {
            value = iter->getNameAsString() + "_" + NameValueNode::_arrayIndex[indexCounter];
            auto* child = FunctionDeclNode::create(iter->getNameAsString().c_str(), iter, value.c_str());
            mocksNode->addChild(child);
         }
      }
   }
   
   return contentArray;
}



void UnitTestData::serializeJson(Json::Value &jsonRoot, const NameValueNode* data) const
{   
   jsonRoot = Json::Value(Json::objectValue);

   if ( data == nullptr ) {
      data =  _treeFromAST.get();
   }
   
   typedef std::pair<Json::Value&, const NameValueNode*> JsonTreeNodePair;
   std::stack< JsonTreeNodePair > Stack;
   
   Stack.push( JsonTreeNodePair(jsonRoot, data) );
   
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
         else if (childNode->isArrayElementObject() ) {
            
            const int index = childNode->getIndex();
            currentJson[index] = Json::Value(Json::objectValue);         
            UnitTestDataUtils::addJsonArrayElementComment(currentJson[index], childNode);
   
            Stack.push( JsonTreeNodePair(currentJson[index], childNode) ); 
         }
         else if (childNode->isArrayElementValue()) {
	    const int index = childNode->getIndex();
            currentJson[index] = childNode->getValue();         
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



void UnitTestData::deSerializeJson(const Json::Value &jsonRoot ) 
{   
      
   typedef std::pair<const Json::Value&, NameValueNode*> JsonTreeNodePair;
   std::stack< JsonTreeNodePair > Stack;
   
   // temp tree node created while parsng the json file
   // it  contains only NameValueNode and no specialized Node (QualTypeNode,FunctionDeclNode)
   // no checks are performed on tempTreeFromJson
   std::unique_ptr<NameValueNode> tempTreeFromJson;
   
   if ( jsonRoot.isObject() == true ) {
      
      tempTreeFromJson = std::unique_ptr<NameValueNode>( NameValueNode::createObject("dataJson") );
   }
   else {
      std::cout << "ERROR: json root value is not an object\n";
      return;
   }

   Stack.push( JsonTreeNodePair(jsonRoot, tempTreeFromJson.get()) );
   
   while ( !Stack.empty() ) {
    
//     check if it's safe get a const reference of the top of the stack and call pop()
      const JsonTreeNodePair& topPair = Stack.top();
      Stack.pop();
      
      const Json::Value& currentJson= topPair.first;
      NameValueNode* currentNode = topPair.second;
      
      if ( currentJson.isNull() || currentJson.empty() )
         continue;
      
      if( currentJson.size() > 0 ) {
            
         Json::ValueConstIterator jsonBeginIter = currentJson.begin();
         Json::ValueConstIterator jsonIter = currentJson.end();
          
         while ( jsonIter-- != jsonBeginIter ) { 
             
            if ( jsonIter.key().isString() ) {
               
               const std::string& name = jsonIter.key().asString();
               
               if ( jsonIter->isArray() ) {

                  NameValueNode* arrayChildNode = NameValueNode::createArray(name.c_str());
                  currentNode->addChild(arrayChildNode);
                  Stack.push( JsonTreeNodePair( *jsonIter, arrayChildNode) );
               }
               else if ( jsonIter->isObject() ) {

                  NameValueNode* objectChildNode = NameValueNode::createObject( name.c_str() );
                  currentNode->addChild(objectChildNode);
                  Stack.push( JsonTreeNodePair( *jsonIter, objectChildNode) );
               }
               else {
                  
                  const char* value = jsonIter->asCString();
//                   std::cout << "key = " << name << "\tvalue = "  << value << std::endl;
                  NameValueNode* valueChildNode = NameValueNode::createValue(name.c_str(), value );
                  currentNode->addChild(valueChildNode); 
               }
            }
            else if ( jsonIter.key().isInt() ) {       
//              array element

                const int index = jsonIter.key().asInt();
                
                if ( jsonIter->isString() ) {
                   
                  const char* value = jsonIter->asCString();
//                   std::cout << "key = " << index << "\tvalue = "  << value << std::endl;
                  NameValueNode* arrayElemChildNode = NameValueNode::createArrayElement(index,  value );
                  currentNode->addChild(arrayElemChildNode);
                }
                else {
                
                   NameValueNode* arrayElemChildNode = NameValueNode::createArrayElement(index );
                   currentNode->addChild(arrayElemChildNode);
                   Stack.push( JsonTreeNodePair( *jsonIter, arrayElemChildNode) );  
                }
            } 
                     
          }                  
      }
      else {
         
         std::cout << "something is wrong in UnitTestData::deSerializeJson while loop";
      }

   } 
   
   // create the data tree _treeData adding specialized node (QualTypeNode,FunctionDeclNode)
   if ( tempTreeFromJson ) {
      
      buildTreeData( tempTreeFromJson.get() );
      
      NameValueNode* globalsTreeData = buildGlobalsTreeData(tempTreeFromJson.get());
      
      if ( globalsTreeData != nullptr ) {
//          add globals tree node branch
         _treeData->addChild(globalsTreeData);
      }
      
   }
   
}


NameValueNode* UnitTestData::createValidatedNode(const NameValueNode* refChildNode,const NameValueNode* jsonNode ) 
{
   NameValueNode* newNode = nullptr;
   
   if ( refChildNode != nullptr) {
//       clone the refChildNode name and type but with the jsonNode value
      newNode = refChildNode->clone(jsonNode->getValue().c_str());
   }
   else {
//       is it a value ?
      newNode = jsonNode->clone();
   }
   
   return newNode;
}


bool UnitTestData::buildTreeData( const NameValueNode* tempTreeFromJson)
{
   
   typedef std::tuple<const NameValueNode*, const NameValueNode*, NameValueNode* > treeNodeTuple;
   std::stack< treeNodeTuple > Stack;
   
   _treeData = std::unique_ptr<NameValueNode>( NameValueNode::createObject("data") );
   
   Stack.push( treeNodeTuple(_treeFromAST.get(), tempTreeFromJson, _treeData.get()) );
   
   while ( !Stack.empty() ) {
      
      const treeNodeTuple& top = Stack.top();
      Stack.pop();
      
      const NameValueNode* currentRefNode = std::get<0>(top);
      const NameValueNode* currentJsonNode = std::get<1>(top);
      NameValueNode* currentDataNode = std::get<2>(top);
      
      if ( currentJsonNode == nullptr )
         continue;
      
      const std::map< std::string, std::unique_ptr<NameValueNode> >& children = currentJsonNode->getChildren();
      for ( auto child=children.rbegin(); child!=children.rend(); ++child) {
         
         const std::string& childJsonName = child->first;
         const NameValueNode* childJsonNode = child->second.get();
         
         const NameValueNode* refChildNode = nullptr;
         if ( ( currentRefNode != nullptr ) && ( currentRefNode->getNumChildern() > 0 ) ) {
            
            refChildNode = currentRefNode->getChild(childJsonName.c_str());
            if ( refChildNode == nullptr ) {
               std::cout << "ERROR: " << childJsonName << " is NOT a valid object\n";
// 	       continue;
            }
         }
         
         if ( childJsonNode->isObject() ) {
            
            NameValueNode* newObjectNode = createValidatedNode(refChildNode, childJsonNode );         
            currentDataNode->addChild(newObjectNode);
            Stack.push( treeNodeTuple( refChildNode, childJsonNode, newObjectNode) );    
         }
         else if ( childJsonNode->isArray() ) {
            
            NameValueNode* newArrayNode = NameValueNode::createArray(childJsonName.c_str());
            currentDataNode->addChild(newArrayNode);
            Stack.push( treeNodeTuple( refChildNode, childJsonNode, newArrayNode) );  
         }
         else if (childJsonNode->isArrayElement() ) {
            
            NameValueNode* newArrayElementNode = NameValueNode::createArrayElement(childJsonNode->getIndex());
            currentDataNode->addChild(newArrayElementNode);
            
            if ( currentJsonNode->getName()=="funcs" || 
                 currentJsonNode->getName()=="mocks") {
               
               const NameValueNode* nameField = childJsonNode->getChild("_name");
               if ( nameField== nullptr ) {
                  throw logic_error("ERROR: array object with index " + childJsonNode->getName() + " has no _name field");
               }
               
                
               const std::map< std::string, std::unique_ptr<NameValueNode> >& currRefNodeChildren = currentRefNode->getChildren();
               for ( const auto& refArrayObjectIter : currRefNodeChildren ) {
                  if ( refArrayObjectIter.second->getChild("_name")->getValue() == nameField->getValue() ) {
                     refChildNode = refArrayObjectIter.second.get();
                     break;
                  }
               }
            }
            
            Stack.push( treeNodeTuple( refChildNode, childJsonNode, newArrayElementNode) );  
         }
         else {
            
            NameValueNode* newDataNode = createValidatedNode(refChildNode, childJsonNode );
            currentDataNode->addChild(newDataNode); 
         }
         
      }
   }
}


NameValueNode*  UnitTestData::buildGlobalsTreeData( const NameValueNode* tempTreeFromJson)
{
   const NameValueNode* tempGlobalsFromJson = tempTreeFromJson->getChild("globals");
   if ( tempGlobalsFromJson == nullptr )
      return false;
   
   NameValueNode* _globalsTreeNodeData = NameValueNode::createArray("globals");
   
   unsigned int arrayIndex = 0;
   
   const std::map< std::string, std::unique_ptr<NameValueNode> >& children = tempGlobalsFromJson->getChildren();
   for ( auto child=children.rbegin(); child!=children.rend(); ++child) {
         
      const NameValueNode* childJsonNode = child->second.get();
      
      if ( childJsonNode->isArrayElementObject() == false ) {
         continue;
      }
      
      NameValueNode* newArrayElementNode = NameValueNode::createArrayElement( arrayIndex );
      _globalsTreeNodeData->addChild(newArrayElementNode);
            
      
      const NameValueNode* typeNode = childJsonNode->getChild("_type");
      if ( typeNode== nullptr ) {
         throw logic_error("ERROR: array object with index " + childJsonNode->getName() + " has no _type field");
      }
      
      const NameValueNode* nameNode = childJsonNode->getChild("_name");
      if ( nameNode== nullptr ) {
         throw logic_error("ERROR: array object with index " + childJsonNode->getName() + " has no _name field");
      }
      // TODO Process Value to strip all qualifiers (const, static, *, ...)
      const auto& qualTypeIter = _allTypesMap->find(typeNode->getValue()) ;
      if ( qualTypeIter == _allTypesMap->end() ) {
         throw logic_error("ERROR: array object with index " + childJsonNode->getName() + " set type " + typeNode->getValue() + " which is not defined");
      }
      
      const NameValueNode* contentJsonNode = childJsonNode->getChild("content");
      if ( contentJsonNode== nullptr ) {
         throw logic_error("ERROR: array object with index " + contentJsonNode->getName() + " has no content field");
      }
      
      
      QualTypeNode* childQualTypeNode = nullptr;
      
      const std::map< std::string, std::unique_ptr<NameValueNode> >& contentChildren = contentJsonNode->getChildren();
      for ( auto contentChild=contentChildren.begin(); contentChild!=contentChildren.end(); ++contentChild)  {
         
         const std::string& typeName =  qualTypeIter->first;
         const clang::QualType& qualType = qualTypeIter->second;
         const NameValueNode* contentChildNode = contentChild->second.get();
         
         const clang::RecordType * structType = qualType->getAsStructureType();

         childQualTypeNode = QualTypeNode::create( typeName.c_str(), qualType );
//             Stack.push( treeNodeTuple( qualType, child.second.get(), childQualTypeNode) ); 
         
         typedef std::tuple< const NameValueNode*, const clang::QualType&, NameValueNode* > treeNodeTuple;
         std::stack< treeNodeTuple > Stack;

         Stack.push( treeNodeTuple( contentChildNode, qualType, childQualTypeNode) );
         
         while ( Stack.empty() )
         {
            const treeNodeTuple& top = Stack.top();
            Stack.pop();
            
            const NameValueNode* currentJsonNode = std::get<0>(top);
            const clang::QualType& refQualType = std::get<1>(top);
            NameValueNode* currentDataNode = std::get<2>(top);
            
            
            const std::string& refQualTypeName = refQualType.getAsString();
            
            const NameValueNode* fieldChildNode = currentJsonNode->getChild( refQualTypeName.c_str() );
            if ( fieldChildNode == nullptr) {                  
               throw logic_error("ERROR:  " + childJsonNode->getName() + " has no _name field");
            }
               
            QualTypeNode* newChildQualTypeNode = nullptr;
            
            const clang::RecordType *structType = refQualType->getAsStructureType();
            if (structType == nullptr) {
               
               newChildQualTypeNode = QualTypeNode::create( refQualTypeName.c_str(), refQualType, fieldChildNode->getValue().c_str() ); 
            }
            else {
               
               newChildQualTypeNode = QualTypeNode::create( refQualTypeName.c_str(), refQualType );
               Stack.push( treeNodeTuple( fieldChildNode, refQualType, newChildQualTypeNode) );                  
            }
            
            currentDataNode->addChild(newChildQualTypeNode);
         }
      
      
         newArrayElementNode->addChild(childQualTypeNode);          
      }
         
   }
      
      
   return _globalsTreeNodeData;
   
   /*
   typedef std::tuple<const QualTypeNode*, const NameValueNode*, NameValueNode* > treeNodeTuple;
   std::stack< treeNodeTuple > Stack;
   
   
   
   
   Stack.push( treeNodeTuple( QualTypeNode::create(), tempTreeFromJson, _treeData.get()) );
   
   while ( !Stack.empty() ) {
      
      const treeNodeTuple& top = Stack.top();
      Stack.pop();
      
      const QualTypeNode* RefQualTypeNode = std::get<0>(top);
      const NameValueNode* currentJsonNode = std::get<1>(top);
      NameValueNode* currentDataNode = std::get<2>(top);
      
      
      const std::map< std::string, std::unique_ptr<NameValueNode> >& children = currentJsonNode->getChildren();
      for (auto child=children.rbegin(); child!=children.rend(); ++child) {
         
         const std::string& childJsonName = child->first;
         const NameValueNode* childJsonNode = child->second.get();
         QualTypeNode* childQualTypeNode = nullptr;
         
         if ( childJsonNode->isObject() ) {
            QualTypeNode* newObjectNode = QualTypeNode::create(childJsonNode->getValue());
            currentDataNode->addChild(newObjectNode);
            Stack.push( treeNodeTuple( refChildNode, childJsonNode, newObjectNode) );    
         }
         else if ( childJsonNode->isArray() ) {
            
            NameValueNode* newArrayNode = NameValueNode::createArray(childJsonName.c_str());
            currentDataNode->addChild(newArrayNode);
            Stack.push( treeNodeTuple( refChildNode, childJsonNode, newArrayNode) );  
         }
         else if (childJsonNode->isArrayElement() ) {
            
            NameValueNode* newArrayElementNode = NameValueNode::createArrayElement(childJsonNode->getIndex());
            currentDataNode->addChild(newArrayElementNode);
            
            if( currentJsonNode->getName()=="globals" ){
               //check and update type
               const NameValueNode* typeField = childJsonNode->getChild("_type");
               if ( typeField== nullptr ) {
                  throw logic_error("ERROR: array object with index " + childJsonNode->getName() + " has no _type field");
               }
               
               const NameValueNode* nameField = childJsonNode->getChild("_name");
               if ( nameField== nullptr ) {
                  throw logic_error("ERROR: array object with index " + childJsonNode->getName() + " has no _name field");
               }
               // TODO Process Value to strip all qualifiers (const, static, *, ...)
               const auto& iter = _allTypesMap->find(typeField->getValue()) ;
               if ( iter == _allTypesMap->end() ) {
                  throw logic_error("ERROR: array object with index " + childJsonNode->getName() + " set type " + typeField->getValue() + " which is not defined");
               }
               
                const NameValueNode* contentField = childJsonNode->getChild("content");
               if ( content== nullptr ) {
                  throw logic_error("ERROR: array object with index " + childJsonNode->getName() + " has no content field");
               }
               
               
               const std::map< std::string, std::unique_ptr<NameValueNode> >& children = contentField->getChildren();
               for ( const auto& child=children.begin(); child!=children.end(); ++child)  {
                  
                  const QualType& qualType = iter->second;
                  const clang::RecordType * structType = qualType->getAsStructureType();
                  if ( structType == nullptr) {
                     childQualTypeNode = QualTypeNode::create( iter->first, qualType, child.second.getValue() ); 
                  }
                  else {
                     childQualTypeNode = QualTypeNode::create( iter->first, qualType );
                     Stack.push( treeNodeTuple( qualType, child.second.get(), childQualTypeNode) );                     
                  }
                  
                  currentDataNode->addChild(newObjectNode); 
                  

                  
               }
               
            }
//             Stack.push( treeNodeTuple( childQualTypeNode, childJsonNode, newArrayElementNode) );  
         }
         else {
            
            NameValueNode* newDataNode = createValidatedNode(refChildNode, childJsonNode );
            currentDataNode->addChild(newDataNode); 
         }
               
   }
   */
   
   
}


void UnitTestData::testActions() const
{
}
