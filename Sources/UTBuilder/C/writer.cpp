
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


#include <template.hpp>

#include "FunctionTestDataFile.h"
#include "utils.h"
#include "UnitTestDataUtils.h"

#include <plustache_types.hpp>

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

using Plustache::Context;
using Plustache::template_t;
using PlustacheTypes::ObjectType;




FileWriterManager::FileWriterManager(const std::string&         fileName,
	    const UnitTestData&           data,
	    const clang::SourceManager&   sourceMgr)
: _fileName(fileName)
, _data(data)
, _sourceMgr(sourceMgr)
{
}

FileWriterManager::~FileWriterManager() = default;

void FileWriterManager::add(BaseWriter* writer) 
{
   if ( writer == nullptr )
      return;
   
   writer->init(_fileName, _data, _sourceMgr);
   _writers.insert( std::unique_ptr<BaseWriter>(writer) );
}

void FileWriterManager::write()
{
   for (const auto& writer : _writers )
   {
      writer->writeTemplate( writer->getTeamplateName(), writer->getTeamplateSuffix() );
   }
}
   
   
   
Writer::Writer(const UnitTestData& data,
               const std::string            &fileName,
               const clang::SourceManager   &sourceMgr)
   : _data(data)
   , _fileName(fileName)
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
//    for (const auto& funcToMock : FunctionsToMock::get().declKeySetMap) {
   for (const auto& funcToMock : _data.getFunctionToMock() ) {
      
      const clang::FunctionDecl *funcDecl = funcToMock.first;

      // get declaration source location
      const string declSrcFile = utils::getDeclSourceFile(funcDecl, _sourceMgr);

      includePaths.insert(path(declSrcFile).filename().string());
   }


   WriteTemplate(std::unique_ptr<const Plustache::Context>( CreateMockContext(includePaths, _data.getFunctionToMock() ) ).get(),
                 string(getenv("TEMPLATE_DIR")) + string("/mock.template"),
                 _fileName + "-mocks.h");

}


void
Writer::createUnitTestFile(void)
{
   string fnameUT = path(_fileName).filename().string();
   set<string> includePaths;

   // look for paths to include in the mock file

   for (const auto& funcToUnitTest : _data.getFunctionToTest() ) { /*FunctionsToUnitTest::get().declKeySetMap) {*/
      const clang::FunctionDecl *funcDecl = funcToUnitTest.first;

      // get declaration source location
      const string declSrcFile = utils::getDeclSourceFile(funcDecl, _sourceMgr);
      path p(declSrcFile);
      includePaths.insert(utils::changeFileExtension(p.filename().string(), "h"));
   }


   WriteTemplate(std::unique_ptr<const Plustache::Context>( CreateUnitTestContext(includePaths, FunctionTestDataFile::get().getUnitTestCollection()) ).get(),
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

   auto serializationContext = std::unique_ptr<const Plustache::Context>( CreateSerializationContext(includePaths, results::get().typedefNameDecls) );
   
   WriteTemplate(serializationContext.get(),
                 std::string(std::getenv("TEMPLATE_DIR")) + std::string("/serialization.template"),
                 _fileName + "-serialization.h");

   auto structToSerializeContext = std::unique_ptr<const Plustache::Context>(CreateStructuresToSerializeContext(includePaths, _data.getFunctionToTest() ) );
   
   WriteTemplate(structToSerializeContext.get(), /* FunctionsToUnitTest::get().declKeySetMap),*/
                 std::string(std::getenv("TEMPLATE_DIR")) + std::string("/serialization-struct.template"),
                 _fileName + "-serialization-struct.h");
}


const Plustache::Context* Writer::CreateMockContext(const std::set<std::string>   &includePaths,
                                                                    const FunctionDeclKeySetMap   &funcDeclsMap)
{
//    std::shared_ptr<Plustache::Context> context = std::make_shared<Plustache::Context>();
   Plustache::Context* context = new Plustache::Context();
   
   ObjectType            Include;
   ObjectType            Mock;
   ObjectType            FakeFunc;
   std::ostringstream    out;


   for (const string& iter : includePaths) {
      Include["include"] = iter;
      context->add("includes", Include);
   }

   for (const auto& iter : funcDeclsMap) {

      const clang::FunctionDecl *funcDecl = iter.first;
      UnitTestDataUtils::writeMockFunctionFFF(funcDecl, &_sourceMgr, out);
      Mock["definition"] = out.str();
      Mock["mockname"] = funcDecl->getNameInfo().getName().getAsString();
      context->add("mocks", Mock);
      out.str("");
   }

   context->add("filename", _fileName);
   context->add("newline", "\n");

   out.str("");

   /*
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
   }*/
   

   const NameValueNode* mocksdata = _data.getMocksData();
   const std::map< std::string, std::unique_ptr<NameValueNode> >& mockFuncs = mocksdata->getChildren();
   for (const auto& arrayIndex : mockFuncs) {
      
      const NameValueNode* childObj = arrayIndex.second.get();
      const FunctionDeclNode* funcName = dynamic_cast<const FunctionDeclNode*>(childObj->getChild("_name"));
      
      if ( funcName == nullptr ){
	
      }
      const std::string& name = funcName->getValue();
      const clang::FunctionDecl* funcDecl = static_cast<const clang::FunctionDecl*>(funcName->getType());
      
      const NameValueNode* funcContent = childObj->getChild("content");
      

      int counter = 0;
      const std::map< std::string, std::unique_ptr<NameValueNode> >& children = funcContent->getChildren();
      for ( const auto& iter : children ) {

         const std::string namewithcounter = name + "_" + std::to_string(counter);
         
         const NameValueNode* outputTree = iter.second->getChild("output");
         UnitTestDataUtils::writeFunctionDefinition(namewithcounter, funcDecl, outputTree, out);
         FakeFunc["definition"] = out.str();
         context->add("fakefuncs", FakeFunc);
         counter++;
         out.str("");
      }      
   }
   
   return context;
}



const Plustache::Context*
Writer::CreateUnitTestContext(const std::set<std::string>   &includePaths,
                              const FunctionTestCollection *   funcData)
{
//    std::shared_ptr<Plustache::Context> context = std::make_shared<Plustache::Context>();
   Plustache::Context* context = new Plustache::Context();
   
   ObjectType   Include;
   ObjectType   FunctionToUnitTest;

   for (const std::string& iter : includePaths) {

      Include["include"] = iter;
      context->add("includes", Include);
   }
   // add mock file in includes list
   Include["include"] = path(_fileName).filename().string() + "-mocks.h";
   context->add("includes", Include);
   Include["include"] = path(_fileName).filename().string() + "-serialization-struct.h";
   context->add("includes", Include);

   std::ostringstream    code;

  /*
   for (const std::pair< std::string, FunctionTestContent>&& iter : funcData->dataFromJson() ) {

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
   */
   
   const NameValueNode* funcsToTest = _data.getFuncsData();
   const std::map< std::string, std::unique_ptr<NameValueNode> >& funcs = funcsToTest->getChildren();
   for (const auto& arrayIndex : funcs) {
      
      const NameValueNode* childObj = arrayIndex.second.get();
      auto funcName = static_cast<const FunctionDeclNode*>(childObj->getChild("_name"));
      const std::string& name = funcName->getValue();
      const clang::FunctionDecl* funcDecl = static_cast<const clang::FunctionDecl*>(funcName->getType());
      
      const NameValueNode* funcContent = childObj->getChild("content");
      
      int counter = 0;
      const std::map< std::string, std::unique_ptr<NameValueNode> >& children = funcContent->getChildren();
      for ( const auto& iter : children ) {
         
         FunctionToUnitTest["functionName"] = name + "_" + std::to_string(counter);;
         ++counter;
         code.str("");
         UnitTestDataUtils::writeGoogleTest(code, funcDecl, iter.second.get() );         
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



const Plustache::Context*
Writer::CreateSerializationContext(const std::set<std::string>                   &includePaths,
                                   const std::set<const clang::TypedefNameDecl *> &typedefNameDecls)
{

//    std::shared_ptr<Plustache::Context> context = std::make_shared<Plustache::Context>();
   Plustache::Context* context = new Plustache::Context();
   
   PlustacheTypes::ObjectType            Include;
   PlustacheTypes::ObjectType            objectToSerialize;


   std::ostringstream    out;

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




const Plustache::Context*
Writer::CreateSerializationStructuresContext(const std::set<std::string> &includePaths,
                                             const FunctionDeclKeySetMap &funcDeclsMap)
{
//    std::shared_ptr<Plustache::Context> context = std::make_shared<Plustache::Context>();
   Plustache::Context* context = new Plustache::Context();
   
   ObjectType            Include;
   ObjectType            paramsStructsObject;
   std::ostringstream    out;


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



const Plustache::Context*
Writer::CreateStructuresToSerializeContext(const std::set<std::string>   &includePaths,
                                           const FunctionDeclKeySetMap   &funcDeclsMap)
{
//    std::shared_ptr<Plustache::Context> context = std::make_shared<Plustache::Context>();
   Plustache::Context* context = new Plustache::Context();
   
   ObjectType            Include;
   ObjectType            paramsStructsObject;
   std::ostringstream    out;

   for (const string& iter : includePaths) {
      Include["include"] = iter;
      context->add("includes", Include);
   }
   //Include["include"] = Include["include"] = utils::changeFileExtension(path(_fileName).filename().string(), "h");
   context->add("includes", Include);

   context->add("filename", _fileName);
   context->add("newline", "\n");


   /*
   std::vector<FunctionTestContent> funcParamsStructures;

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
   */

   for (const std::pair<const clang::FunctionDecl *, FunctionDeclSet >& iter : funcDeclsMap) {
      
      out.str("");
      
      const clang::FunctionDecl *funcDecl = iter.first;
      UnitTestDataUtils::writeFunctionDeclAsStructure( out, funcDecl);
    
      paramsStructsObject["functionName"] = funcDecl->getNameAsString();
      paramsStructsObject["paramTypesAndNames"] = out.str();
      context->add("functionParamsStructs", paramsStructsObject);
   }
   
   return context;
}


void
Writer::WriteTemplate(const Plustache::Context*  context,
                      const std::string                         &templateFileName,
                      const std::string                         &outFileName)

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

   std::ofstream outputFile;
   std::string outputFileName = outFileName;
   outputFile.open(outputFileName, std::fstream::out);
   if ( outputFile.is_open() ){
     outputFile << result;
     std::cout << "file written: " << outputFileName << std::endl;
   }
   else {
     std::cout << "ERROR: opening path to write: " << outputFileName << " doesn't exist" << std::endl;

   outputFile.close();
   }

}

/*
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
   outputFile.close();

   
}
*/


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


