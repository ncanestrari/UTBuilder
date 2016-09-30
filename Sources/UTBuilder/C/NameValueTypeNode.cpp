#include "NameValueTypeNode.h"



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
