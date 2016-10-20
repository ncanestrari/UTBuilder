#include "NameValueTypeNode.h"


// the order of static vars initialization is always respected for each translation unit 
const char* NameValueNode::_arrayIndex[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
const unsigned int NameValueNode::_arrayIndexSize = (sizeof((NameValueNode::_arrayIndex))/sizeof(*(NameValueNode::_arrayIndex)));

// template specialization for clang::QualType type
// QualType can be a structure and addChild is called recursevly for each struct field
template <>
NameValueNode* TypeNameValueNode<clang::QualType>::create(const char* name,  
                                                                 clang::QualType qualType, 
                                                                 const char* value ) 
{
   TypeNameValueNode<clang::QualType>* node = nullptr; // = new TypeNameValueNode<clang::QualType>(name, qualType, value);
   
   const clang::RecordType *structType = qualType->getAsStructureType();
   if (structType != nullptr) {
      // not useful , value can be "" for struct 
      const std::string structName = qualType.getAsString();
      
      node = new TypeNameValueNode<clang::QualType>(name, qualType, structName.c_str() );
  
      const clang::RecordDecl *structDecl = structType->getDecl();
      for (const auto field : structDecl->fields()) {
         auto* child = TypeNameValueNode<clang::QualType>::create(field->getNameAsString().c_str(), field->getType(), "");
         node->addChild(child);
      }
   }
   else {
    
      node = new TypeNameValueNode<clang::QualType>(name, qualType, value );
   }
   
   
   return node;
}
   
   
// template specialization for clang::QualType type
// QualType can be a structure and addChild is called recursevly for each struct field
template <>
std::shared_ptr<NameValueTypeNode<clang::QualType> > NameValueTypeNode<clang::QualType>::addChild(const char *name, clang::QualType qualType, const char *value)
{
   std::shared_ptr<NameValueTypeNode<clang::QualType> > child;
   
   const clang::RecordType *structType = qualType->getAsStructureType();
   if (structType != nullptr) {
      // not useful , value can be "" for struct 
      const std::string structName = qualType.getAsString();
      
      child = std::make_shared<NameValueTypeNode<clang::QualType> >(name, qualType, structName.c_str() );
  
      const clang::RecordDecl *structDecl = structType->getDecl();
      for (const auto field : structDecl->fields()) {
         child->addChild(field->getNameAsString().c_str(), field->getType(), "");
      }
   }
   else {
    
      child = std::make_shared<NameValueTypeNode<clang::QualType> >(name, qualType, value );
   }
   
   _children[name] = child;
   return child;
}
