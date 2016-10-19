
#include "FunctionTestContent.h"

#include <clang/AST/Decl.h>

#include <assert.h>


FunctionTestContent::FunctionTestContent()
: _funcDecl(nullptr)
{}


FunctionTestContent::FunctionTestContent(const FunctionTestContent& other)
: _funcDecl(other._funcDecl)
, _tests(std::vector< std::shared_ptr<FunctionTestData> >( other._tests ))
{
}

// TODO experimenting...
// FunctionTestContent::FunctionTestContent(FunctionTestContent&& other)
// : _funcDecl(other._funcDecl)
// , _tests(std::move( other._tests ))
// {
// }

FunctionTestContent::FunctionTestContent(const clang::FunctionDecl *funcDecl,
             const std::set<const clang::FunctionDecl *> &mockFuncs )
{
   init(funcDecl, mockFuncs);
}

FunctionTestContent::~FunctionTestContent()
{}


FunctionTestContent& FunctionTestContent::operator = (const FunctionTestContent& other)
{
   _funcDecl = other.getFunctionDecl();
   _tests = std::vector< std::shared_ptr<FunctionTestData> >( other.getTests() );
   return *this;
}


void FunctionTestContent::init(const clang::FunctionDecl *funcDecl, const std::set<const clang::FunctionDecl *> &mockFuncs)
{
   clear();

   _funcDecl = funcDecl;
   
   _tests.push_back( std::make_shared<FunctionTestData>(funcDecl, mockFuncs) );
   _tests.push_back( std::make_shared<FunctionTestData>(funcDecl, mockFuncs) );
}


void FunctionTestContent::clear()
{
   _funcDecl = nullptr;

   _tests.clear();
}


std::string FunctionTestContent::getName(const unsigned int index) const
{
   if (index >= getNumTests()) {
      return std::string();
   }

   std::ostringstream sstream;
   sstream << _funcDecl->getNameAsString() << "_" << index;
   return sstream.str();
}


unsigned int FunctionTestContent::getNumTests(void) const
{
   return _tests.size();
}


const std::vector< std::shared_ptr<FunctionTestData> >& FunctionTestContent::getTests(void) const
{ 
   return _tests;
}

std::shared_ptr<FunctionTestData> FunctionTestContent::getTest(const unsigned int idx) const
{
   if (idx >= getNumTests()) 
   {
      return std::shared_ptr<FunctionTestData>(nullptr);
   }
   
   return _tests[idx];
}


void FunctionTestContent::serializeJson(Json::Value &jsonParent) const
{
   Json::Value jsonChild;
   jsonChild["_name"] =  getName();
   jsonChild["content"] = Json::Value(Json::arrayValue);

   for (unsigned int i = 0; i < _tests.size(); ++i) {
      // force "input" to be an object
      jsonChild["content"][i] = Json::Value(Json::objectValue);
      _tests[i]->serializeJson(jsonChild["content"][i]);
   }
   

   // add comment int the created file
   std::ostringstream comment;
   for (unsigned int i = 0; i < _tests.size(); ++i) {
      comment.str(std::string());
      comment <<  "// " << getName() << "_test_" << i;
      jsonChild["content"][i].setComment(comment.str().c_str(), comment.str().length(), Json::commentBefore);
   }

   jsonParent.append(jsonChild);
}



std::shared_ptr<NameValueTypeNode<clang::QualType> >
FunctionTestContent::deSerializeTreeJson(const std::shared_ptr<NameValueTypeNode<clang::QualType> > referenceTree,
                                      const Json::Value &fieldItem)
{
   const std::string &treeKeyName = referenceTree->getName();
   std::shared_ptr<NameValueTypeNode<clang::QualType> > root; // = std::make_shared<NameValueTypeNode<clang::QualType> >(treeKeyName.c_str(), referenceTree->getType());

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
FunctionTestContent::deSerializeTreeJson(const std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > referenceTree, const Json::Value &fieldItem)
{

   const std::string &treeKeyName = referenceTree->getName();

   std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > root;
   
   if (fieldItem.isObject() == true) {
      
       root = std::make_shared<NameValueTypeNode<const clang::FunctionDecl *> >(treeKeyName.c_str(), referenceTree->getType());
         
      for (Json::ValueConstIterator iter = fieldItem.begin() ; iter != fieldItem.end() ; iter++) {

         auto childReferenceTree = referenceTree->getChild(iter.key().asString().c_str());
         std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > child = deSerializeTreeJson(childReferenceTree, *iter);
         root->addChild(child);
      }
   } else {
      root = std::make_shared<NameValueTypeNode<const clang::FunctionDecl *> >(treeKeyName.c_str(), referenceTree->getType(), fieldItem.asString().c_str());
   }

   return root;
}


void FunctionTestContent::deSerializeJson(const Json::Value &jsonRoot, const void *refData )
{
   
   const FunctionTestContent *funcTestContentfromAST = static_cast<const FunctionTestContent *>(refData);
   
   const Json::Value& content = jsonRoot["content"];
   const unsigned int size = content.size();

   _funcDecl = funcTestContentfromAST->getFunctionDecl();
   
   _tests.resize(size);

   std::shared_ptr<const FunctionTestData> funcTestReference = funcTestContentfromAST->getTest(0);
   
   for (int i = 0; i < size; ++i) {
       const Json::Value& jsonTest = content[i];
       std::shared_ptr<FunctionTestData> test = std::make_shared<FunctionTestData>(nullptr);
       test->deSerializeJson( jsonTest, funcTestReference.get() );
       _tests[i] = test;
   }
}


static const char *getStructureField(std::ostringstream &os, const clang::QualType &qualType, const std::string &indent)
{
   const std::string newIndent = indent + indent;
   const clang::RecordType *structType = qualType->getAsStructureType();
   size_t pos = 0;

   std::string typestr = qualType.getUnqualifiedType().getAsString();
   pos = typestr.find("*", pos);
//    while (pos != std::string::npos) {
//       typestr = typestr.erase(pos, 1);
//       pos = typestr.find("*", pos);
//    }

   return typestr.c_str();
}

void  FunctionTestContent::writeAsStructure(std::ostringstream &os, const FunctionTestContent &obj)
{
   const std::string indent = "   ";

   os << "\n";

   if (obj.getNumParams() > 0) {
      for (const auto& field : obj.getFunctionDecl()->params()) {
         clang::QualType qualType = field->getType();
         const clang::QualType canonicalQualType = qualType->getCanonicalTypeInternal();

         os << indent << getStructureField(os, qualType, indent) << "\t" << field->getNameAsString() << "; \n"; // "<< canonicalQualType.getAsString() << "\n";
      }
   }
}


static void writeStructureValue(std::ostringstream &os,
                                       const std::shared_ptr<NameValueTypeNode<clang::QualType> > tree,
                                       const std::string &name,
                                       const std::string &indent)
{
   std::string structName = tree->isArrayElement() ? name + "[" + tree->getName() + "]" : name + tree->getName();
   
   if ( tree->isArray() ) {
//    this is a pointer to allocate: write the memory allocation line
      if (tree->getNumChildern() > 0) {
         // move in utils::
         size_t pos = 0;
         std::string typestr = tree->getType().getUnqualifiedType().getAsString();
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
         writeStructureValue(os, child.second, structName, indent);
      }
   } else {
      if (tree->getValue() != "") {
         os << indent << structName << " = " << tree->getValue() << ";\n";
      }
   }

   return;
}

static void writeStructureComparison(std::ostringstream &os,
                                            const std::shared_ptr<NameValueTypeNode<clang::QualType> > tree,
                                            const std::string &name,
                                            const std::string &indent)
{
   std::string structName = tree->isArrayElement() ? name + "[" + tree->getName() + "]" : name + tree->getName();

   if (tree->getNumChildern() > 0) {

      if ( !tree->isArray() )
         structName += ".";

      for (const auto& child : tree->getChildren()) {
         writeStructureComparison(os, child.second, structName, indent);
      }
   } else {
      if (tree->getValue() != "") {
         os << indent << "EXPECT_EQ(" << structName << ", " << tree->getValue() << ");\n";
      }
   }

}

void FunctionTestContent::writeGoogleTest(std::ostringstream &os, const FunctionTestContent &obj, const unsigned int i)
{
   const std::string indent = "   ";

   std::string structName(obj.getName());
   structName.append("_params");


   //    input declaration
   os << "// input structure declaration" << "\n";
   os << indent << structName << " input;" << "\n";
   os << indent << "memset( &input, 0, sizeof(" << structName << ") );" << "\n\n";

   
   os << "// fill the input struct with json file values" << "\n";
   if (obj.getNumParams() > 0) {
      writeStructureValue(os, obj.getTests()[i]->getInputTree(), "", indent);
   }
   
   os << "\n";
      
   //    mocks No recursion in mock tree
   const auto& children = obj.getTest(i)->getMockTree()->getChildren();
   if ( children.size() > 0)
      os << "// initialize mock functions\n";
   for (const auto& iter : children) {
      const std::string &value = iter.second->getValue();
      if (value != "") {
         os << indent << iter.first << "_fake.custom_fake = " << value << ";\n";
         //TODO fill the context with necessary manually written mocks fill the custom_fake with the content
      }
   }

   os << "\n";

   //    output declaration
   const clang::QualType returnType = obj.getFunctionDecl()->getReturnType();
   const std::string &returnTypeString = returnType.getAsString();

   if (returnTypeString != "void") {
      os << indent << returnTypeString << " retval;" << "\n";
      os << indent << "retval = ";
   }


   os << obj.getName() << "(";

   if (obj.getNumParams() > 0) {
      const int numParms = obj.getFunctionDecl()->getNumParams();
      const clang::ParmVarDecl *currentParam = obj.getFunctionDecl()->getParamDecl(0);

      os  << " input." << currentParam->getNameAsString();
      for (int ii = 1; ii < numParms; ++ii) {
         const clang::ParmVarDecl *currentParam = obj.getFunctionDecl()->getParamDecl(ii);
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
   for (const auto& child : obj.getTests()[i]->getOutputTree()->getChildren()) {
      if (child.first == "retval") {
         writeStructureComparison(os, child.second, "", indent);
      } else {
         writeStructureComparison(os, child.second, "input.", indent);
      }
   }
   
//    os << ");\n\n";
//    os << "// free allocated memory" << "\n";
//    if (obj.getNumParams() > 0) {
//       os << writeFreeMemory(os, obj.getTests()[i]->getInputTree(), "", indent) << "\n";
//    }
   
}


