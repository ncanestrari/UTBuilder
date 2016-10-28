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

void UnitTestDataUtils::writeGoogleTest(std::ostringstream &os, const clang::FunctionDecl* funcDecl, const NameValueNode* contentElement ) 
{
 
   const std::string indent = "   ";

   const std::string name = funcDecl->getNameAsString(); 
   const std::string structName = name + "_params";
//    structName.append("_params");


   //    input declaration
   os << "// input structure declaration" << "\n";
   os << indent << structName << " input;" << "\n";
   os << indent << "memset( &input, 0, sizeof(" << structName << ") );" << "\n\n";

   
   os << "// fill the input struct with json file values" << "\n";
   if (funcDecl->getNumParams() > 0) {
      const NameValueNode* inputTree = contentElement->getChild("input");
      writeStructureValue(os, inputTree, "", indent);
   }
   
   os << "\n";
      
   //    mocks No recursion in mock tree
   const NameValueNode* mocksTree = contentElement->getChild("mock-funcs-call");
   const auto& mocksChildren = mocksTree->getChildren();

   if ( mocksChildren.size() > 0)
      os << "// initialize mock functions\n";
   for (const auto& iter : mocksChildren) {
      const std::string &value = iter.second->getValue();
      if (value != "") {
         os << indent << iter.first << "_fake.custom_fake = " << value << ";\n";
         //TODO fill the context with necessary manually written mocks fill the custom_fake with the content
      }
   }

   os << "\n";

   //    output declaration
   const clang::QualType returnType = funcDecl->getReturnType();
   const std::string &returnTypeString = returnType.getAsString();

   if (returnTypeString != "void") {
      os << indent << returnTypeString << " retval;" << "\n";
      os << indent << "retval = ";
   }


   os << name << "(";

   if ( funcDecl->getNumParams() > 0) {
      const int numParms = funcDecl->getNumParams();
      const clang::ParmVarDecl *currentParam = funcDecl->getParamDecl(0);

      os  << " input." << currentParam->getNameAsString();
      for (int ii = 1; ii < numParms; ++ii) {
         const clang::ParmVarDecl *currentParam = funcDecl->getParamDecl(ii);
         if (currentParam->getOriginalType()->isAnyPointerType()) { 
            //need to be improved
//             os  << ", &input." << currentParam->getNameAsString();
            os  << ", input." << currentParam->getNameAsString();
         } else {
            os  << ", input." << currentParam->getNameAsString();
         }
      }
   }
   os << ");\n\n";

   
   os << "// check conditions" << "\n";
   
   const NameValueNode* outputTree = contentElement->getChild("output");
   const auto& outputChildren = outputTree->getChildren();
   
   for (const auto& child : outputChildren ) {
      if (child.first == "retval") {
         writeStructureComparison(os, child.second.get(), "", indent);
      } else {
         writeStructureComparison(os, child.second.get(), "input.", indent);
      }
   }
}


void UnitTestDataUtils::writeStructureValue( std::ostringstream &os,
                                             const NameValueNode* tree,
                                             const std::string &name,
                                             const std::string &indent)
{
   std::string structName = tree->isArrayElement() ? name + "[" + tree->getName() + "]" : name + tree->getName();
   
   if ( tree->isArray() ) {
//    this is a pointer to allocate: write the memory allocation line
      if (tree->getNumChildern() > 0) {
         // move in utils::
         size_t pos = 0;
         clang::QualType type = *static_cast<clang::QualType*>(tree->getType());
         std::string typestr = type.getUnqualifiedType().getAsString();
         pos = typestr.find("*", pos);
         while (pos != std::string::npos) {
            typestr = typestr.erase(pos, 1);
            pos = typestr.find("*", pos);
         }

         os << indent << structName << " = alloca(" << tree->getNumChildern() << "*sizeof(" << typestr << "));\n";
         os << indent << "memset(&" << structName <<" ,0, " << tree->getNumChildern() << "*sizeof(" << typestr << "));\n";
      }
   }

   if (tree->getNumChildern() > 0) {
      
      if ( !tree->isArray() )
         structName += ".";
      
      for (const auto& child : tree->getChildren()) {
         writeStructureValue(os, child.second.get(), structName, indent);
      }
   } else {
      if (tree->getValue() != "") {
         os << indent << structName << " = " << tree->getValue() << ";\n";
      }
   }

   return;
}


void UnitTestDataUtils::writeStructureComparison(std::ostringstream &os,
                                                 const NameValueNode* tree,
                                                 const std::string &name,
                                                 const std::string &indent )
{
   std::string structName = tree->isArrayElement() ? name + "[" + tree->getName() + "]" : name + tree->getName();

   if (tree->getNumChildern() > 0) {

      if ( !tree->isArray() )
         structName += ".";

      for (const auto& child : tree->getChildren()) {
         writeStructureComparison(os, child.second.get(), structName, indent);
      }
   } else {
      if (tree->getValue() != "") {
         os << indent << "EXPECT_EQ(" << structName << ", " << tree->getValue() << ");\n";
      }
   }

}