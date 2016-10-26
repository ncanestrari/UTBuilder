#include "UnitTestDataUtils.h"

#include "NameValueTypeNode.h"

#include <clang/AST/Decl.h>



NameValueNode* UnitTestDataUtils::findContentFromAST(const std::string &key, const NameValueNode* dataAST)
{
   NameValueNode* retNode = nullptr;
   
   const std::map< std::string, std::unique_ptr<NameValueNode> >& mocksRefChildren = dataAST->getChildren();
   std::map< std::string, std::unique_ptr<NameValueNode> >::const_iterator iter = mocksRefChildren.begin();
   for ( iter; iter == mocksRefChildren.end(); ++iter ) {
      if ( iter->first == key )
         break;
   }
   if ( iter != mocksRefChildren.end() )
   {
      retNode = iter->second.get();
   }
   return retNode;
}


void UnitTestDataUtils::addJsonArrayComment(Json::Value& jsonValue, const NameValueNode* node)
{
}

void UnitTestDataUtils::addJsonArrayElementComment(Json::Value& jsonValue, const NameValueNode* node)
{
   std::ostringstream comment;
   const int index = node->getIndex();
   comment <<  "// \"_name\"_test_" << index;
   jsonValue.setComment(comment.str().c_str(), comment.str().length(), Json::commentBefore);
}

void UnitTestDataUtils::addJsonObjectComment(Json::Value& jsonValue, const NameValueNode* node)
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

void UnitTestDataUtils::addJsonValueComment(Json::Value& jsonValue, const NameValueNode* node)
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

void UnitTestDataUtils::addJsonStructValueComment(Json::Value& jsonValue, const NameValueNode* node)
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



static const char * getStructureFieldRecursive(std::ostringstream &os, const clang::QualType &qualType, const std::string &indent)
{
   const std::string newIndent = indent + indent;
   const clang::RecordType *structType = qualType->getAsStructureType();
   size_t pos = 0;

   std::string typestr = qualType.getUnqualifiedType().getAsString();
//    pos = typestr.find("*", pos);
//    while (pos != std::string::npos) {
//       typestr = typestr.erase(pos, 1);
//       pos = typestr.find("*", pos);
//    }

   return typestr.c_str();
}

void UnitTestDataUtils::writeFunctionDeclAsStructure(std::ostringstream &os, const clang::FunctionDecl* funcDecl )
{
   const std::string indent = "   ";

   os << "\n";

   if (funcDecl->getNumParams() > 0) {
      for (const auto& field : funcDecl->params()) {
         clang::QualType qualType = field->getType();
         const clang::QualType canonicalQualType = qualType->getCanonicalTypeInternal();

         os << indent << getStructureFieldRecursive(os, qualType, indent) << "\t" << field->getNameAsString() << "; \n"; // "<< canonicalQualType.getAsString() << "\n";
      }
   }
}