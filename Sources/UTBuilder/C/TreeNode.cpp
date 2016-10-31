#include "TreeNode.h"


// the order of static vars initialization is always respected for each translation unit 
template <typename Derived, typename T>
const char* BaseNode<Derived,T>::_arrayIndex[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

template <typename Derived, typename T>
const unsigned int BaseNode<Derived,T>::_arrayIndexSize = (sizeof((BaseNode::_arrayIndex))/sizeof(*(BaseNode::_arrayIndex)));

template <typename Derived, typename T>
const char* BaseNode<Derived,T>::_arrayElementObject = "arrayElement";


const int Node::_default = 0;
const clang::QualType QualTypeNode::_default = clang::QualType();
clang::FunctionDecl* const FunctionDeclNode::_default = nullptr;


// template specialization for clang::QualType type
// QualType can be a structure and addChild is called recursevly for each struct field

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

