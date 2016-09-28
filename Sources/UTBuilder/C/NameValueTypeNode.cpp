
#include "NameValueTypeNode.h"


// specialization for clang::QualType type
template <>
std::shared_ptr<NameValueTypeNode<clang::QualType> > NameValueTypeNode<clang::QualType>::addChild(const char *name, clang::QualType qualType, const char *value)
{
   auto child = std::make_shared<NameValueTypeNode<clang::QualType> >(name, qualType, value);
   const clang::RecordType *structType = qualType->getAsStructureType();
   if (structType != nullptr) {
      const clang::RecordDecl *structDecl = structType->getDecl();
      child->_value = qualType.getAsString();
      for (const auto field : structDecl->fields()) {
         child->addChild(field->getNameAsString().c_str(), field->getType(), "");
      }
   }
   _children[name] = child;

   return child;
}

