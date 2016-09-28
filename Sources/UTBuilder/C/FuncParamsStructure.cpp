
#include "FuncParamsStructure.h"

#include <clang/AST/Decl.h>

#include <assert.h>


FuncParamsStruct::FuncParamsStruct()
{}


FuncParamsStruct::~FuncParamsStruct()
{}


void FuncParamsStruct::init(const clang::FunctionDecl *funcDecl, const std::set<const clang::FunctionDecl *> &mockFuncs)
{
   clear();

   _funcDecl = funcDecl;

   _inputTree.push_back(buildInputTree(funcDecl));
   _inputTree.push_back(buildInputTree(funcDecl));
   _outputTree.push_back(buildOutputTree(funcDecl));
   _outputTree.push_back(buildOutputTree(funcDecl));

   _mocksTree.push_back(buildMockFuncsTree(mockFuncs));
   _mocksTree.push_back(buildMockFuncsTree(mockFuncs));

}


void FuncParamsStruct::clear()
{
   _funcDecl = nullptr;

   _inputTree.clear();
   _outputTree.clear();
   _mocksTree.clear();

}


std::string FuncParamsStruct::getName(const unsigned int index) const
{
   if (index >= getSize()) {
      return std::string();
   }

   std::ostringstream sstream;
   sstream << _funcDecl->getNameAsString() << "_" << index;
   return sstream.str();
}


unsigned int FuncParamsStruct::getSize(void) const
{
   const unsigned int size = _outputTree.size();
   assert(size == _inputTree.size());
   assert(size == _mocksTree.size());
   return size;
}


std::shared_ptr<NameValueTypeNode<clang::QualType> > FuncParamsStruct::buildInputTree(const clang::FunctionDecl *funcDecl)
{

   std::shared_ptr<NameValueTypeNode<clang::QualType> > root = std::make_shared<NameValueTypeNode<clang::QualType> >("input");

   if (funcDecl->getNumParams() > 0) {
      for (auto field : funcDecl->params()) {
         root->addChild(field->getNameAsString().c_str(), field->getType(), "");
      }
   }

   return root;
}


std::shared_ptr<NameValueTypeNode<clang::QualType> > FuncParamsStruct::buildOutputTree(const clang::FunctionDecl *funcDecl)
{
   std::shared_ptr<NameValueTypeNode<clang::QualType> > root = buildInputTree(funcDecl);

   //    only change name
   root->setName("output");

   // add return type
   root->addChild("retval", funcDecl->getReturnType(), "");

   return root;
}


std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > FuncParamsStruct::buildMockFuncsTree(const std::set<const clang::FunctionDecl *> &mockFuncs)
{
   std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > root = std::make_shared<NameValueTypeNode<const clang::FunctionDecl *> >("mock-funcs-call");

   std::string value;
   for (auto iter : mockFuncs) {
      value = iter->getNameAsString() + "[0]";
      root->addChild(iter->getNameAsString().c_str(), iter, value.c_str());
   }

   return root;
}


void FuncParamsStruct::serializeTree(std::shared_ptr<NameValueTypeNode<clang::QualType> > tree, Json::Value &fieldItem)
{
   std::string comment;

   const std::string &keyName = tree->getName();

   if (tree->getNumChildern() > 0) {
      for (auto child : tree->getChildren()) {
         serializeTree(child.second, fieldItem[keyName]);
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


void FuncParamsStruct::serializeTree(std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > tree, Json::Value &fieldItem)
{
   std::string comment; // = "// defined in mocks-json file";
   const std::string &keyName = tree->getName();
   if (tree->getNumChildern() > 0) {
      for (auto child : tree->getChildren()) {
         serializeTree(child.second, fieldItem[keyName]);
      }
   } else {
      fieldItem[keyName] = tree->getValue();
   }
}


void FuncParamsStruct::serialize(Json::Value &jsonParent,  const bool withMocks)
{
   Json::Value jsonChild;
   jsonChild["_name"] =  getName();
   jsonChild["content"] = Json::Value(Json::arrayValue);

   for (unsigned int i = 0; i < _inputTree.size(); ++i) {
      // force "input" to be an object
      jsonChild["content"][i]["input"] = Json::Value(Json::objectValue);
      serializeTree(_inputTree[i], jsonChild["content"][i]);
   }

   for (unsigned int i = 0; i < _outputTree.size(); ++i) {
      // force "output" to be an object
      jsonChild["content"][i]["output"] = Json::Value(Json::objectValue);
      serializeTree(_outputTree[i], jsonChild["content"][i]);
   }


   if (withMocks) {
      for (int i = 0; i < _mocksTree.size(); ++i) {
         // force "mock-funcs-call" to be an object
         jsonChild["content"][i]["mock-funcs-call"] = Json::Value(Json::objectValue);
         serializeTree(_mocksTree[i], jsonChild["content"][i]);
      }
   }

   //       add comment int the created file
   std::ostringstream comment;
   for (unsigned int i = 0; i < _inputTree.size(); ++i) {
      comment.str(std::string());
      comment <<  "// " << getName() << "_test_" << i;
      jsonChild["content"][i].setComment(comment.str().c_str(), comment.str().length(), Json::commentBefore);
   }

   jsonParent.append(jsonChild);
}


void FuncParamsStruct::deSerializeTree(std::shared_ptr<NameValueTypeNode<clang::QualType> > tree, const Json::Value &fieldItem)
{
   const std::string &treeKeyName = tree->getName();
   Json::Value field = fieldItem.get(treeKeyName.c_str(), "");
   if (field.isObject() == true) {
      for (Json::ValueIterator iter = field.begin() ; iter != field.end() ; iter++) {
         const std::string key = iter.key().asString();
         auto child = tree->getChild(key.c_str());
         deSerializeTree(child, field);
      }
   } else {
      tree->setValue(field.asCString());
   }
}


void FuncParamsStruct::deSerializeTree(std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > tree, const Json::Value &fieldItem)
{

   if (fieldItem.isObject()) {
      const std::string &treeKeyName = tree->getName();
      Json::Value field = fieldItem.get(treeKeyName.c_str(), "");

      if (field.isObject() == true) {
         for (Json::ValueIterator iter = field.begin() ; iter != field.end() ; iter++) {
            const std::string key = iter.key().asString();
            auto child = tree->getChild(key.c_str());
            deSerializeTree(child, *iter);
         }

      }
   } else {
      tree->setValue(fieldItem.asCString());
   }
}

void FuncParamsStruct::deSerialize(const Json::Value &jsonRoot)
{
   const Json::Value content = jsonRoot["content"];
   const unsigned int size = content.size();

   _inputTree.resize(size);
   _outputTree.resize(size);
   _mocksTree.resize(size);

   for (int i = 0; i < size; ++i) {
      deSerializeTree(_inputTree[i], content[i]);
      deSerializeTree(_outputTree[i], content[i]);
      deSerializeTree(_mocksTree[i], content[i]);
   }
}

std::shared_ptr<NameValueTypeNode<clang::QualType> >
FuncParamsStruct::deSerializeTreeJson(const std::shared_ptr<NameValueTypeNode<clang::QualType> > referenceTree,
                                      const Json::Value &fieldItem)
{
   const std::string &treeKeyName = referenceTree->getName();
   std::shared_ptr<NameValueTypeNode<clang::QualType> > root = std::make_shared<NameValueTypeNode<clang::QualType> >(treeKeyName.c_str());
   root->setType(referenceTree->getType());

   if (fieldItem.isObject() == true) {
      //Json::Value field = fieldItem.get(treeKeyName.c_str(), "");
      for (Json::ValueConstIterator iter = fieldItem.begin() ; iter != fieldItem.end() ; iter++) {

         auto childReferenceTree = referenceTree->getChild(iter.key().asString().c_str());
         std::shared_ptr<NameValueTypeNode<clang::QualType> > child = deSerializeTreeJson(childReferenceTree, *iter);
         root->addChild(child);
      }
   } else {
      root->setValue(fieldItem.asString().c_str());
   }

   return root;
}

std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> >
FuncParamsStruct::deSerializeTreeJson(const std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > referenceTree, const Json::Value &fieldItem)
{

   const std::string &treeKeyName = referenceTree->getName();

   std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > root = std::make_shared<NameValueTypeNode<const clang::FunctionDecl *> >(treeKeyName.c_str());

   if (fieldItem.isObject() == true) {
      //Json::Value field = fieldItem.get(treeKeyName.c_str(), "");
      for (Json::ValueConstIterator iter = fieldItem.begin() ; iter != fieldItem.end() ; iter++) {

         auto childReferenceTree = referenceTree->getChild(iter.key().asString().c_str());
         std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > child = deSerializeTreeJson(childReferenceTree, *iter);
         root->addChild(child);
      }
   } else {
      root->setValue(fieldItem.asString().c_str());
      root->setType(referenceTree->getType());
   }

   return root;
}


void FuncParamsStruct::deSerializeJson(const FuncParamsStruct &funcParam, const Json::Value &jsonRoot)
{
   const Json::Value content = jsonRoot["content"];
   const unsigned int size = content.size();

   _inputTree.resize(size);
   _outputTree.resize(size);
   _mocksTree.resize(size);

   for (int i = 0; i < size; ++i) {
      _inputTree[i] = deSerializeTreeJson(funcParam._inputTree[i], content[i]["input"]);
      _outputTree[i] = deSerializeTreeJson(funcParam._outputTree[i], content[i]["output"]);
      _mocksTree[i] = deSerializeTreeJson(funcParam._mocksTree[i], content[i]["mock-funcs-call"]);
   }
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

void  FuncParamsStruct::writeAsStructure(std::ostringstream &os, const FuncParamsStruct &obj)
{
   const std::string indent = "   ";

   os << "\n";

   if (obj.getNumParams() > 0) {
      for (auto field : obj.getFunctionDecl()->params()) {
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
      for (auto child : tree->getChildren()) {
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

      for (auto child : tree->getChildren()) {
         writeStructureComparison(os, child.second, structName, indent);
      }
   } else {
      if (tree->getValue() != "") {
         os << indent << "EXPECT_EQ(" << structName << ", " << tree->getValue() << ");\n";
      }
   }

   return "";
}

void FuncParamsStruct::writeGoogleTest(std::ostringstream &os, const FuncParamsStruct &obj, const unsigned int i)
{
   const std::string indent = "   ";

   std::string structName(obj.getName());
   structName.append("_params");


   //    input declaration
   os << indent << structName << " input;" << "\n";
   os << indent << "memset( &input, 0, sizeof(" << structName << ") );" << "\n\n";

   //    mocks No recursion in mock tree
   auto children = obj._mocksTree[i]->getChildren();
   for (auto iter : children) {
      const std::string &value = iter.second->getValue();
      if (value != "") {
         os << indent << iter.first << "_fake.custom_fake = " << value << ";\n";
         //TODO fill the context with necessary manually written mocks fill the custom_fake with the content
      }
   }

   //    output declaration
   const clang::QualType returnType = obj.getFunctionDecl()->getReturnType();
   const std::string &returnTypeString = returnType.getAsString();

   if (returnTypeString != "void") {
      os << indent << returnTypeString << " retval;" << "\n";
   }

   os << "// fill the input struct with json file values" << "\n";
   if (obj.getNumParams() > 0) {
      os << writeStructureValue(os, obj._inputTree[i], "", indent) << "\n";
   }

   os << "\n" <<  indent;

   if (returnTypeString != "void") {
      os << "retval = ";
   }

   os << obj.getName() << "(";

   if (obj.getNumParams() > 0) {
      const int numParms = obj.getFunctionDecl()->getNumParams();
      const clang::ParmVarDecl *currentParam = obj.getFunctionDecl()->getParamDecl(0);

      os  << " input." << currentParam->getNameAsString();
      for (int ii = 1; ii < numParms; ++ii) {
         const clang::ParmVarDecl *currentParam = obj.getFunctionDecl()->getParamDecl(ii);
         if (currentParam->getOriginalType()->isAnyPointerType()) { //need to be inproved
            os  << ", &input." << currentParam->getNameAsString();
         } else {
            os  << ", input." << currentParam->getNameAsString();
         }
      }
   }
   os << ");\n\n";

   os << "// check conditions" << "\n";
   for (auto child : obj._outputTree[i]->getChildren()) {
      if (child.first == "retval") {
         writeStructureComparison(os, child.second, "", indent);
      } else {
         writeStructureComparison(os, child.second, "input.", indent);
      }
   }
}


