#include "TreeNode.h"




const NodeType Node::_defaultType = _value_type;

// the order of static vars initialization is always respected for each translation unit 
const char* Node::_arrayIndex[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

const unsigned int Node::_arrayIndexSize = (sizeof((Node::_arrayIndex))/sizeof(*(Node::_arrayIndex)));

const char* Node::_arrayElementObject = "arrayElement";


// const clang::QualType QualTypeNode::_defaultType = clang::QualType();
// const clang::FunctionDecl* const FunctionDeclNode::_defaultType = nullptr;


// template specialization for clang::QualType type
// QualType can be a structure and addChild is called recursevly for each struct field
/*
QualTypeNode* QualTypeNode::create(const char* name, clang::QualType qualType, const char* value ) 
{
   QualTypeNode* node = nullptr;
   
   const clang::RecordType *structType = qualType->getAsStructureType();
   if (structType != nullptr) {
      // not useful , value can be "" for struct 
      const std::string structName = qualType.getAsString();
      
      node = new QualTypeNode(name, qualType, structName.c_str() );
  
      const clang::RecordDecl *structDecl = structType->getDecl();
      for (const auto field : structDecl->fields()) {
         auto* child = QualTypeNode::create(field->getNameAsString().c_str(), field->getType(), "");
         node->addChild(child);
      }
   }
   else {
    
      node = new QualTypeNode(name, qualType, value );
   }
   
   
   return node;
}
*/
