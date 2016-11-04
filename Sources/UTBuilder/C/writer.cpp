
#include "writer.h"

#include <boost/filesystem/convenience.hpp>
using boost::filesystem::current_path;
using boost::filesystem::path;

#include <clang/AST/Decl.h>
#include <clang/AST/PrettyPrinter.h>
using clang::SourceManager;

using clang::BuiltinType;
using clang::EnumType;
using clang::QualType;
using clang::RecordType;
using clang::TypedefType;

using clang::EnumDecl;
using clang::FunctionDecl;
using clang::ParmVarDecl;
using clang::RecordDecl;
using clang::TypedefNameDecl;

#include <cstdlib>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using std::cout;
using std::endl;
using std::fstream;
using std::getenv;
using std::ifstream;
using std::logic_error;
using std::make_shared;
using std::ofstream;
using std::ostringstream;
using std::pair;
using std::set;
using std::shared_ptr;
using std::string;
using std::stringstream;
using std::to_string;
using std::vector;

#include <plustache_types.hpp>
#include <template.hpp>
using Plustache::Context;
using Plustache::template_t;
using PlustacheTypes::ObjectType;

#include "FunctionTestDataFile.h"
#include "utils.h"



Writer::Writer(const string            &fileName,
               const SourceManager   &sourceMgr)
   : _fileName(fileName)
   , _sourceMgr(sourceMgr)
{}


void Writer::createFiles(void)
{
   CreateMockFile();
   CreateSerializationFile();
   createUnitTestFile();
}


void
Writer::CreateMockFile(void)
{
   set<string> includePaths;

   // look for paths to include in the mock file
   for (const auto& funcToMock : FunctionsToMock::get().declKeySetMap) {
      const FunctionDecl *funcDecl = funcToMock.first;
      // get declaration source location
      const string declSrcFile = utils::getDeclSourceFile(funcDecl, _sourceMgr);

      includePaths.insert(path(declSrcFile).filename().string());
   }

   WriteTemplate(CreateMockContext(includePaths, FunctionsToMock::get().declKeySetMap),
                 string(getenv("TEMPLATE_DIR")) + string("/mock.template"),
                 _fileName + "-mocks.h");
}


void
Writer::createUnitTestFile(void)
{
   string fnameUT = path(_fileName).filename().string();
   set<string> includePaths;

   // look for paths to include in the mock file
   for (const auto& funcToUnitTest : FunctionsToUnitTest::get().declKeySetMap) {
      const FunctionDecl *funcDecl = funcToUnitTest.first;
      // get declaration source location
      const string declSrcFile = utils::getDeclSourceFile(funcDecl, _sourceMgr);
      path p(declSrcFile);
      includePaths.insert(utils::changeFileExtension(p.filename().string(), "h"));
   }

   WriteTemplate(CreateUnitTestContext(includePaths, FunctionTestDataFile::get().getUnitTestCollection()),
                 string(getenv("TEMPLATE_DIR")) + string("/UT.template"),
                 _fileName + "-ugtest.cpp");
}


void
Writer::CreateSerializationFile(void)
{
   ostringstream out;
   path fpathUT(_fileName);
   string fnameUT = fpathUT.filename().string();
   set<string> includePaths;

   for (const auto& typedefDecl : results::get().typedefNameDecls) {
      // get declaration source location
      const string declSrcFile = utils::getDeclSourceFile(typedefDecl, _sourceMgr);
      const string includeFile =  path(declSrcFile).filename().string();
      if (!includeFile.empty()) {
         includePaths.insert(includeFile);
      }
   }

   for (const auto& structDecl : results::get().structDecls) {
      // get declaration source location
      const string declSrcFile = utils::getDeclSourceFile(structDecl, _sourceMgr);
      const string includeFile =  path(declSrcFile).filename().string();
      if (!includeFile.empty()) {
         includePaths.insert(includeFile);
      }
   }

   WriteTemplate(CreateSerializationContext(includePaths, results::get().typedefNameDecls),
                 string(getenv("TEMPLATE_DIR")) + string("/serialization.template"),
                 _fileName + "-serialization.h");

   WriteTemplate(CreateStructuresToSerializeContext(includePaths, FunctionsToUnitTest::get().declKeySetMap),
                 string(getenv("TEMPLATE_DIR")) + string("/serialization-struct.template"),
                 _fileName + "-serialization-struct.h");
}


shared_ptr<const Context>
Writer::CreateMockContext(const set<string> &           includePaths,
                          const FunctionDeclKeySetMap & funcDeclsMap)
{
   shared_ptr<Context> context = make_shared<Context>();
   ObjectType Include;
   ObjectType Mock;
   ObjectType FakeFunc;
   ostringstream out;

   for (const string& iter : includePaths) {
      Include["include"] = iter;
      context->add("includes", Include);
   }

   for (const auto& iter : funcDeclsMap) {

      const FunctionDecl *funcDecl = iter.first;
      MockFunctionFFF(funcDecl, out);
      Mock["definition"] = out.str();
      Mock["mockname"] = funcDecl->getNameInfo().getName().getAsString();
      context->add("mocks", Mock);
      out.str("");
   }

   context->add("filename", _fileName);
   context->add("newline", "\n");

   out.str("");

   const FunctionTestCollection * mockFuncTestCollection = FunctionTestDataFile::get().getMockTestCollection();

   for ( const pair<string, FunctionTestContent>& func : mockFuncTestCollection->dataFromJson() ) {

      const string &name = func.first;
      const FunctionTestContent &funcParams = func.second;
      const FunctionDecl *funcDecl = funcParams.getFunctionDecl();
      int counter = 0;
      const vector<shared_ptr<FunctionTestData> >& tests = funcParams.getTests();
      const unsigned int size = funcParams.getNumTests();
      
      for (int i = 0; i < size; ++i ) {
         const string namewithcounter = name + "_" + to_string(counter);
         FakeFunctionDefinition(namewithcounter, funcDecl, tests[i]->getOutputTree(), out);
         FakeFunc["definition"] = out.str();
         context->add("fakefuncs", FakeFunc);
         counter++;
         out.str("");
      }
   }

   return context;
}


shared_ptr<const Context>
Writer::CreateUnitTestContext(const set<string> &              includePaths,
                              const FunctionTestCollection *   funcData)
{
   shared_ptr<Context> context = make_shared<Context>();
   ObjectType Include;
   ObjectType FunctionToUnitTest;
   ostringstream code;

   for (const string& iter : includePaths) {
      Include["include"] = iter;
      context->add("includes", Include);
   }
   // add mock file in includes list
   Include["include"] = path(_fileName).filename().string() + "-mocks.h";
   context->add("includes", Include);
   Include["include"] = path(_fileName).filename().string() + "-serialization-struct.h";
   context->add("includes", Include);
  
   for (const pair<string, FunctionTestContent>& iter : funcData->dataFromJson() ) {

      const FunctionTestContent& funcParams = iter.second;
      const unsigned int size = funcParams.getNumTests();

      for (int i = 0; i < size; ++i) {
         FunctionToUnitTest["functionName"] = funcParams.getName(i); // funcDecl->getNameAsString();
         code.str("");
         FunctionTestContent::writeGoogleTest(code, funcParams, i);
         FunctionToUnitTest["CODE"] = code.str();
         context->add("functionToUnitTest", FunctionToUnitTest);
      }
   }

   // create a C++ class name from the fileName
   string TestFilename = path(_fileName).filename().string();
   TestFilename = utils::removeDashes(TestFilename);
   TestFilename = utils::removeFileExtension(TestFilename);

   context->add("testFilename", TestFilename);
   context->add("filename", _fileName);
   context->add("newline", "\n");

   return context;
}


shared_ptr<const Context>
Writer::CreateSerializationContext(const set<string> &                  includePaths,
                                   const set<const TypedefNameDecl *> & typedefNameDecls)
{
   shared_ptr<Context> context = make_shared<Context>();
   ObjectType Include;
   ObjectType objectToSerialize;
   ostringstream out;

   context->add("filename", _fileName);
   context->add("newline", "\n");
   context->add("indent", "   ");

   for (const string& iter : includePaths) {
      Include["include"] = iter;
      context->add("includes", Include);
   }

   for (const auto& typedefDecl : typedefNameDecls) {
      objectToSerialize.clear();

      const QualType typedefQualType = typedefDecl->getUnderlyingType(); // ->getCanonicalTypeInternal();
      // appends the row and column to the name string
      const string declSrcFile = utils::getDeclSourceFileLine(typedefDecl, _sourceMgr);

      objectToSerialize["name"] = typedefDecl->getNameAsString();
      objectToSerialize["file"] = declSrcFile;

      if (const RecordType *structType = typedefQualType->getAsStructureType()) {
         const RecordDecl *structDecl = structType->getDecl();

         objectToSerialize["typedefStructName"] = structDecl->getNameAsString();

         out.str("");
         for (const auto& field : structDecl->fields()) {
            out << "   " << field->getType().getAsString() << "\t" << field->getNameAsString() << ";\n";
         }

         objectToSerialize["fieldsToSerialize"] = out.str();
         objectToSerialize["typedefStructDeclName"] = typedefDecl->getNameAsString();

         context->add("structsToSerialize", objectToSerialize);

      } else if (const EnumType *enumType = typedefQualType->getAs<EnumType>()) {
         const EnumDecl *enumDecl = enumType->getDecl();
         objectToSerialize["typedefEnumName"] = enumDecl->getNameAsString();

         out.str("");
         for (const auto& iter : enumDecl->enumerators()) {
            out << "   " << iter->getNameAsString() << "\t = " << iter->getInitVal().toString(10) << ",\n";
         }

         objectToSerialize["valuesToSerialize"] = out.str();

         const string qualTypeName = typedefQualType.getAsString();
         const QualType canonicalQualType = typedefQualType->getCanonicalTypeInternal();
         const string canonicalTypeName = canonicalQualType.getAsString();

         objectToSerialize["typedefEnumDeclName"] = canonicalTypeName;

         context->add("enumsToSerialize", objectToSerialize);
      }  else if (const TypedefType *typedefType = typedefQualType->getAs<TypedefType>()) {

         const QualType canonicalQualType = typedefType->getCanonicalTypeInternal();
         const string canonicalTypeName = canonicalQualType.getAsString();

         objectToSerialize["typedefType"] = typedefQualType.getAsString();
         objectToSerialize["typedefDeclName"] = typedefDecl->getNameAsString();

         context->add("typedefToSerialize", objectToSerialize);
      } else if (const BuiltinType *typedefType = typedefQualType->getAs<BuiltinType>()) {
         objectToSerialize["typedefType"] = typedefQualType.getAsString();
         objectToSerialize["typedefDeclName"] = typedefDecl->getNameAsString();

         context->add("typedefToSerialize", objectToSerialize);
      }
   }

   return context;
}



shared_ptr<const Context>
Writer::CreateSerializationStructuresContext(const set<string> &            includePaths,
                                             const FunctionDeclKeySetMap &  funcDeclsMap)
{
   shared_ptr<Context> context = make_shared<Context>();
   ObjectType Include;
   ObjectType paramsStructsObject;
   ostringstream out;

   for (const string& iter : includePaths) {
      Include["include"] = iter;
      context->add("includes", Include);
   }
   //include
   Include["include"] = utils::changeFileExtension(path(_fileName).filename().string(), "h");
   context->add("includes", Include);

   //    context->add("testFilename", TestFilename);
   context->add("filename", _fileName);
   context->add("newline", "\n");

   // write function support structures
   for (const auto& iter : funcDeclsMap) {
      const FunctionDecl *funcDecl = iter.first;
      // get declaration source location
      paramsStructsObject["functionName"] = funcDecl->getNameInfo().getName().getAsString();

      string returnType = funcDecl->getReturnType().getAsString();
      paramsStructsObject["returnType"] = returnType;

      out.str("");
      const int numParms = funcDecl->getNumParams();

      for (int i = 0; i < numParms; ++i) {
         const ParmVarDecl *_currentParam = funcDecl->getParamDecl(i);
         out << _currentParam->getType().getUnqualifiedType().getAsString() << "\t" << _currentParam->getQualifiedNameAsString() << ";\n   ";
      }

      paramsStructsObject["paramTypesAndNames"] = out.str();
      context->add("functionParamsStructs", paramsStructsObject);
   }

   return context;
}


shared_ptr<const Context>
Writer::CreateStructuresToSerializeContext(const set<string> &            includePaths,
                                           const FunctionDeclKeySetMap &  funcDeclsMap)
{
   shared_ptr<Context> context = make_shared<Context>();

   ObjectType Include;
   ObjectType paramsStructsObject;
   ostringstream out;

   for (const string& iter : includePaths) {
      Include["include"] = iter;
      context->add("includes", Include);
   }
   //Include["include"] = Include["include"] = utils::changeFileExtension(path(_fileName).filename().string(), "h");
   context->add("includes", Include);

   context->add("filename", _fileName);
   context->add("newline", "\n");

   vector<FunctionTestContent> funcParamsStructures;

   for (const pair<const FunctionDecl *, FunctionDeclSet >& iter : funcDeclsMap) {
      const FunctionDecl *funcDecl = iter.first;
      const set<const FunctionDecl *> &mockDeclSet = iter.second;
      funcParamsStructures.push_back( FunctionTestContent(funcDecl, mockDeclSet) );
   }

   for (const FunctionTestContent& iter : funcParamsStructures) {
      out.str("");
      FunctionTestContent::writeAsStructure(out, iter);

      paramsStructsObject["functionName"] = iter.getName();
      paramsStructsObject["paramTypesAndNames"] = out.str();
      context->add("functionParamsStructs", paramsStructsObject);
   }

   return context;
}


void
Writer::WriteTemplate(shared_ptr<const Context>  context,
                      const string &             templateFileName,
                      const string &             outFileName)
{
   template_t t;
   stringstream buffer;
   string result;

   path full_path(current_path());
   ifstream template_file(templateFileName);

   if (template_file.fail()) {
      throw logic_error("Attempt to open template file\n\n\t" + templateFileName + "\n\nFailed\n");
   }

   buffer << template_file.rdbuf();
   string template_buff(buffer.str());

   result = t.render(template_buff, *context);

   ofstream outputFile;
   string outputFileName = outFileName;
   outputFile.open(outputFileName, fstream::out);
   outputFile << result;
   outputFile.close();

   cout << "file written: " << outputFileName << endl;
}


static void
writeMockValue(ostringstream &                               os,
               const shared_ptr<NameValueTypeNode<QualType>> tree,
               const string &                                name)
{
   string structName = tree->isArrayElement() ? name + "[" + tree->getName() + "]" : name + tree->getName();

   if ( tree->isArray() ) {
//    this is a pointer to allocate: write the memory allocation line
      if (tree->getNumChildern() > 0) {
         // move in utils::
         size_t pos = 0;
         string typestr = tree->getType().getUnqualifiedType().getAsString();
         pos = typestr.find("*", pos);
         while (pos != string::npos) {
            typestr = typestr.erase(pos, 1);
            pos = typestr.find("*", pos);
         }
      }
   }
   
   if (tree->getNumChildern() > 0) {
      if ( !tree->isArray() )
         structName += ".";
      
      for (const auto& child : tree->getChildren()) {
         writeMockValue(os, child.second, structName);
      }
      
   } else { //TODO manage pointer to structure if needed
      
      if (tree->getValue() != "") {
         if (tree->getName() == "retval") {
            os << "   retval = " << tree->getValue() << ";\n";
         } else if (tree->getType()->isAnyPointerType()) {
            os << "   " << structName << " = " << tree->getValue() << ";\n";
         } else {
            os << "   " << structName << " = " << tree->getValue() << ";\n";
         }
      }
   }
}


void
Writer::FakeFunctionDefinition(const string &                                 name,
                               const FunctionDecl *                           funcDecl,
                               const shared_ptr<NameValueTypeNode<QualType>>  outTree,
                               ostringstream &                                out)
{
   string returnType = funcDecl->getReturnType().getAsString();
   string isVariadic;

   out << returnType << " ";
   out << name << "(";

   const int numParms = funcDecl->getNumParams();
   if (numParms == 0) {
      out << "void";
   } else {
      const ParmVarDecl *_currentParam = funcDecl->getParamDecl(0);
      out << _currentParam->getType().getAsString() << " " << _currentParam->getNameAsString();
      for (int i = 1; i < numParms; ++i) {
         const ParmVarDecl *_currentParam = funcDecl->getParamDecl(i);
         out << ", " << _currentParam->getType().getAsString() << " " << _currentParam->getNameAsString();
      }

      if (funcDecl->isVariadic()) {
         out << ", ...";
      }
   }

   out << " ){";

   out << "// fill the input struct with json file values" << "\n";


   for (const auto& child : outTree->getChildren()) {
      if ( (child.first == "retval") && 
           ! (child.second->getType().getAsString() == "void") ) {
         out << "   " << child.second->getType().getAsString() << " retval;\n";
         if ( !child.second->getType()->isAnyPointerType() ) {
            out << "   memset(&retval,0,sizeof(" << child.second->getType().getAsString() << "));\n";
         }
      }
      writeMockValue(out, child.second, "");
   }

   if (returnType != "void") {
      out << "   return retval;\n";
   }

   out << "}";
}


void
Writer::MockFunctionFFF(const FunctionDecl *  funcDecl,
                        ostringstream &       out)
{
   const string declSrcFile = utils::getDeclSourceFileLine(funcDecl, _sourceMgr);

   out << "/**" << endl;
   out << " * name: " << funcDecl->getNameInfo().getName().getAsString() << endl;
   out << " * file: " << declSrcFile << endl;
   out << " */" << endl;

   string returnType = funcDecl->getReturnType().getAsString();
   string isVariadic;

   if (funcDecl->isVariadic()) {
      isVariadic = "_VARARG";
   }

   if (returnType == "void") {
      out << "FAKE_VOID_FUNC" << isVariadic << "( ";
   } else {
      out << "FAKE_VALUE_FUNC" << isVariadic << "( " << returnType << ", ";
   }

   out << funcDecl->getNameInfo().getName().getAsString();

   const int numParms = funcDecl->getNumParams();
   for (int i = 0; i < numParms; ++i) {
      const ParmVarDecl *_currentParam = funcDecl->getParamDecl(i);
      out << ", " << _currentParam->getType().getAsString();
   }

   if (funcDecl->isVariadic()) {
      out << ", ...";
   }

   out << " );";

   out << endl;
}

