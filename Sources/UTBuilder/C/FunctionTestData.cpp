
#include "FunctionTestData.h"

#include <clang/AST/Decl.h>

#include <assert.h>


FunctionTestData::FunctionTestData( const clang::FunctionDecl *funcDecl, const std::set<const clang::FunctionDecl *> &mockFuncs)
{
   if ( funcDecl != nullptr )
   {
      _inputTree = buildInputTree(funcDecl);
      _outputTree = buildOutputTree(funcDecl);
      _mocksTree = buildMockFuncsTree(mockFuncs);
   }
   else {
      // error management! 
   }
   
}


FunctionTestData::~FunctionTestData()
{}


void FunctionTestData::clear()
{
}


std::shared_ptr<NameValueTypeNode<clang::QualType> > FunctionTestData::buildInputTree(const clang::FunctionDecl *funcDecl)
{

   std::shared_ptr<NameValueTypeNode<clang::QualType> > root = std::make_shared<NameValueTypeNode<clang::QualType> >("input");

   if (funcDecl->getNumParams() > 0) {
      for (const auto& field : funcDecl->params()) {
         root->addChild(field->getNameAsString().c_str(), field->getType(), "");
      }
   }

   return root;
}


std::shared_ptr<NameValueTypeNode<clang::QualType> > FunctionTestData::buildOutputTree(const clang::FunctionDecl *funcDecl)
{

   std::shared_ptr<NameValueTypeNode<clang::QualType> > root = std::make_shared<NameValueTypeNode<clang::QualType> >("output");

   if (funcDecl->getNumParams() > 0) {
      for (const auto& field : funcDecl->params()) {
         root->addChild(field->getNameAsString().c_str(), field->getType(), "");
      }
   }

   // add return type
   root->addChild("retval", funcDecl->getReturnType(), "");

   return root;
}


std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > FunctionTestData::buildMockFuncsTree(const std::set<const clang::FunctionDecl *> &mockFuncs)
{
   std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > root = std::make_shared<NameValueTypeNode<const clang::FunctionDecl *> >("mock-funcs-call");

   std::string value;
   for (const clang::FunctionDecl* iter : mockFuncs) {
      value = iter->getNameAsString() + "_0";
      root->addChild(iter->getNameAsString().c_str(), iter, value.c_str());
   }

   return root;
}


void FunctionTestData::serializeJsonTree(std::shared_ptr<NameValueTypeNode<clang::QualType> > tree, Json::Value &fieldItem)
{
   std::string comment;

   const std::string &keyName = tree->getName();

   if (tree->getNumChildern() > 0) {
      for (const auto& child : tree->getChildren()) {
         serializeJsonTree(child.second, fieldItem[keyName]);
      }

      // add comment to json
      const clang::QualType qualType = tree->getType();
      if (qualType.isNull() == false) {
         comment = "// struct " + qualType.getAsString();
         fieldItem[tree->getName()].setComment(comment.c_str(), comment.length(), Json::commentAfterOnSameLine);
      }
   // don't set any value for already created objects ( input ,output and mock-funcs-call )
   }  else if (fieldItem[keyName].isObject() == false) {
      fieldItem[keyName] = tree->getValue();

      // add comment to json
      const clang::QualType qualType = tree->getType();
      if (qualType.isNull() == false) {
         const clang::QualType canonicalQualType = qualType->getCanonicalTypeInternal();
         comment = "// type: " + qualType.getAsString() + " (" + canonicalQualType.getAsString() + ") ";
         fieldItem[tree->getName()].setComment(comment.c_str(), comment.length(), Json::commentAfterOnSameLine);
      }
   }
}


void FunctionTestData::serializeJsonTree(std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > tree, Json::Value &fieldItem)
{
   std::string comment; // = "// defined in mocks-json file";
   const std::string &keyName = tree->getName();
   if (tree->getNumChildern() > 0) {
      for (const auto& child : tree->getChildren()) {
         serializeJsonTree(child.second, fieldItem[keyName]);
      }
   } else {
      fieldItem[keyName] = tree->getValue();
   }
}


void FunctionTestData::serializeJson(Json::Value &jsonParent) const
{   
   serializeJsonTree(_inputTree, jsonParent);
   serializeJsonTree(_outputTree, jsonParent);
   serializeJsonTree(_mocksTree, jsonParent);
}


std::shared_ptr<NameValueTypeNode<clang::QualType> >
FunctionTestData::deSerializeTreeJson(const std::shared_ptr<NameValueTypeNode<clang::QualType> > referenceTree,
                                      const Json::Value &fieldItem)
{
   const std::string &treeKeyName = referenceTree->getName();
   std::shared_ptr<NameValueTypeNode<clang::QualType> > root;

   if (fieldItem.isObject() == true) {
      
      root = std::make_shared<NameValueTypeNode<clang::QualType> >(treeKeyName.c_str(), referenceTree->getType());

      for (Json::ValueConstIterator iter = fieldItem.begin() ; iter != fieldItem.end() ; iter++) {

         auto childReferenceTree = referenceTree->getChild(iter.key().asString().c_str());
         std::shared_ptr<NameValueTypeNode<clang::QualType> > child = deSerializeTreeJson(childReferenceTree, *iter);
         root->addChild(child);
      }
   } else {
      
      root = std::make_shared<NameValueTypeNode<clang::QualType> >(treeKeyName.c_str(), referenceTree->getType(), fieldItem.asString().c_str());

   }

   return root;
}

std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> >
FunctionTestData::deSerializeTreeJson(const std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > referenceTree, const Json::Value &fieldItem)
{

   const std::string &treeKeyName = referenceTree->getName();

   std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > root;

   if (fieldItem.isObject() == true) {
      
       root = std::make_shared<NameValueTypeNode<const clang::FunctionDecl *> >(treeKeyName.c_str(), referenceTree->getType());
         
      for (Json::ValueConstIterator iter = fieldItem.begin() ; iter != fieldItem.end() ; iter++) {

         auto childReferenceTree = referenceTree->getChild(iter.key().asString().c_str());
         
         // error check
         if ( childReferenceTree.get() == nullptr ) {
            std::cout << "error building mock-funcs-call" << std::endl;
            return root;
         }
         
         std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > child = deSerializeTreeJson(childReferenceTree, *iter);
         root->addChild(child);
      }
   } else {
      root = std::make_shared<NameValueTypeNode<const clang::FunctionDecl *> >(treeKeyName.c_str(), referenceTree->getType(), fieldItem.asString().c_str());
   }

   return root;
}


void FunctionTestData::deSerializeJson(const Json::Value &jsonParent, const void *refData )
{
//    data from AST used as reference for validation 
   const FunctionTestData *refDataAST = static_cast<const FunctionTestData *>(refData);
   
   const Json::Value & inputValue = jsonParent["input"];
   const Json::Value & outputValue = jsonParent["output"];
   const Json::Value & mocksValue = jsonParent["mock-funcs-call"];
   
   _inputTree = deSerializeTreeJson(refDataAST->_inputTree, inputValue);
   _outputTree = deSerializeTreeJson(refDataAST->_outputTree, outputValue);
   _mocksTree = deSerializeTreeJson(refDataAST->_mocksTree, mocksValue);
   
}


static const char *getStructureField(std::ostringstream &os, const clang::QualType &qualType, const std::string &indent)
{
   const std::string newIndent = indent + indent;
   const clang::RecordType *structType = qualType->getAsStructureType();
   size_t pos = 0;

   std::string typestr = qualType.getUnqualifiedType().getAsString();
   pos = typestr.find("*", pos);
   while (pos != std::string::npos) {
      typestr = typestr.erase(pos, 1);
      pos = typestr.find("*", pos);
   }

   return typestr.c_str();
}

void  FunctionTestData::writeAsStructure(std::ostringstream &os, const clang::FunctionDecl *funcDecl)
{
   const std::string indent = "   ";

   os << "\n";

   if (funcDecl->getNumParams() > 0) {
      for (const auto& field : funcDecl->params()) {
         clang::QualType qualType = field->getType();
         const clang::QualType canonicalQualType = qualType->getCanonicalTypeInternal();

         os << indent << getStructureField(os, qualType, indent) << "\t" << field->getNameAsString() << "; \n"; // "<< canonicalQualType.getAsString() << "\n";
      }
   }
}


static const char *writeStructureValue(std::ostringstream &os,
                                       const std::shared_ptr<NameValueTypeNode<clang::QualType> > tree,
                                       const std::string &name,
                                       const std::string &indent)
{
   std::string structName = name + tree->getName();
   if (tree->getNumChildern() > 0) {
      structName += ".";
      for (const auto& child : tree->getChildren()) {
         writeStructureValue(os, child.second, structName, indent);
      }
   } else {
      if (tree->getValue() != "") {
         os << indent << structName << " = " << tree->getValue() << ";\n";
      }
   }

   return "";
}

static const char *writeStructureComparison(std::ostringstream &os,
                                            const std::shared_ptr<NameValueTypeNode<clang::QualType> > tree,
                                            const std::string &name,
                                            const std::string &indent)
{
   std::string structName = name + tree->getName();

   if (tree->getNumChildern() > 0) {

      structName += ".";

      for (const auto& child : tree->getChildren()) {
         writeStructureComparison(os, child.second, structName, indent);
      }
   } else {
      if (tree->getValue() != "") {
         os << indent << "EXPECT_EQ(" << structName << ", " << tree->getValue() << ");\n";
      }
   }

   return "";
}

void FunctionTestData::writeGoogleTest(std::ostringstream &os, const clang::FunctionDecl *funcDecl )
{
   const std::string indent = "   ";

   std::string name = funcDecl->getNameAsString();
   std::string structName( name );
   structName.append("_params");


   //    input declaration
   os << indent << structName << " input;" << "\n";
   os << indent << "memset( &input, 0, sizeof(" << structName << ") );" << "\n\n";

   //    mocks No recursion in mock tree
   auto children = _mocksTree->getChildren();
   for (const auto& iter : children) {
      const std::string &value = iter.second->getValue();
      if (value != "") {
         os << indent << iter.first << "_fake.custom_fake = " << value << ";\n";
         //TODO fill the context with necessary manually written mocks fill the custom_fake with the content
      }
   }

   //    output declaration
   const clang::QualType returnType = funcDecl->getReturnType();
   const std::string &returnTypeString = returnType.getAsString();

   if (returnTypeString != "void") {
      os << indent << returnTypeString << " retval;" << "\n";
   }

   os << "// fill the input struct with json file values" << "\n";
   if (funcDecl->getNumParams() > 0) {
      os << writeStructureValue(os, _inputTree, "", indent) << "\n";
   }

   os << "\n" <<  indent;

   if (returnTypeString != "void") {
      os << "retval = ";
   }

   os << name << "(";

   if (funcDecl->getNumParams() > 0) {
      const int numParms = funcDecl->getNumParams();
      const clang::ParmVarDecl *currentParam = funcDecl->getParamDecl(0);

      os  << " input." << currentParam->getNameAsString();
      for (int i = 1; i < numParms; ++i) {
         const clang::ParmVarDecl *currentParam = funcDecl->getParamDecl(i);
         if (currentParam->getOriginalType()->isAnyPointerType()) { //need to be inproved
            os  << ", &input." << currentParam->getNameAsString();
         } else {
            os  << ", input." << currentParam->getNameAsString();
         }
      }
   }
   os << ");\n\n";

   os << "// check conditions" << "\n";
   for (const auto& child : _outputTree->getChildren()) {
      if (child.first == "retval") {
         writeStructureComparison(os, child.second, "", indent);
      } else {
         writeStructureComparison(os, child.second, "input.", indent);
      }
   }
}


