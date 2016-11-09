
#include "writer.h"

#include <boost/filesystem/convenience.hpp>
#include <clang/AST/Decl.h>
#include <clang/AST/PrettyPrinter.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <plustache_types.hpp>
#include <sstream>
#include <string>
#include <template.hpp>

#include "FunctionTestDataFile.h"
#include "utils.h"
#include "UnitTestDataUtils.h"

using std::cout;
using std::string;
using std::ifstream;
using std::stringstream;
using PlustacheTypes::ObjectType;
using PlustacheTypes::CollectionType;
using Plustache::Context;
using Plustache::template_t;



FilesWriter::FilesWriter(const std::string&         fileName,
	    const UnitTestData&           data,
	    const clang::SourceManager&   sourceMgr)
: _fileName(fileName)
, _data(data)
, _sourceMgr(sourceMgr)
{
}

FilesWriter::~FilesWriter() = default;

void FilesWriter::add(BaseWriter* writer) 
{
   if ( writer == nullptr )
      return;
   
   writer->init(_fileName, _data, _sourceMgr);
   _writers.insert( std::unique_ptr<BaseWriter>(writer) );
}

void FilesWriter::write()
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


void Writer::CreateMockFile(void)
{

   std::set<std::string> includePaths;

   // look for paths to include in the mock file
//    for (const auto& funcToMock : FunctionsToMock::get().declKeySetMap) {
   for (const auto& funcToMock : _data.getFunctionToMock() ) {
      
      const clang::FunctionDecl *funcDecl = funcToMock.first;
      // get declaration source location
      const std::string declSrcFile = utils::getDeclSourceFile(funcDecl, _sourceMgr);

      includePaths.insert(boost::filesystem::path(declSrcFile).filename().string());
   }

   WriteTemplate(std::unique_ptr<const Plustache::Context>( CreateMockContext(includePaths, _data.getFunctionToMock() ) ).get(), /*FunctionsToMock::get().declKeySetMap),*/
                 std::string(std::getenv("TEMPLATE_DIR")) + std::string("/mock.template"),
                 utils::changeFilePathToInclude(_fileName) + "-mocks.h");

}


void Writer::createUnitTestFile(void)
{

   std::string fnameUT = boost::filesystem::path(_fileName).filename().string();

   std::set<std::string> includePaths;

   // look for paths to include in the mock file
   for (const auto& funcToUnitTest : _data.getFunctionToTest() ) { /*FunctionsToUnitTest::get().declKeySetMap) {*/
      const clang::FunctionDecl *funcDecl = funcToUnitTest.first;
      // get declaration source location
      const std::string declSrcFile = utils::getDeclSourceFile(funcDecl, _sourceMgr);
      boost::filesystem::path p(declSrcFile);
      includePaths.insert(utils::changeFileExtension(p.filename().string(), "h"));
   }


   WriteTemplate(std::unique_ptr<const Plustache::Context>( CreateUnitTestContext(includePaths, FunctionTestDataFile::get().getUnitTestCollection()) ).get(),
                 std::string(std::getenv("TEMPLATE_DIR")) + std::string("/UT.template"),
                 _fileName + "-ugtest.cpp");

}


void Writer::CreateSerializationFile(void)
{
   std::ostringstream out;
   boost::filesystem::path fpathUT(_fileName);
   std::string fnameUT = fpathUT.filename().string();
   std::set<std::string> includePaths;

   for (const auto& typedefDecl : results::get().typedefNameDecls) {
      // get declaration source location
      const std::string declSrcFile = utils::getDeclSourceFile(typedefDecl, _sourceMgr);
      const std::string includeFile =  boost::filesystem::path(declSrcFile).filename().string();
      if (!includeFile.empty()) {
         includePaths.insert(includeFile);
      }
   }

   for (const auto& structDecl : results::get().structDecls) {
      // get declaration source location
      const std::string declSrcFile = utils::getDeclSourceFile(structDecl, _sourceMgr);
      const std::string includeFile =  boost::filesystem::path(declSrcFile).filename().string();
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
                 utils::changeFilePathToInclude(_fileName) + "-serialization-struct.h");
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

   for (const std::string& iter : includePaths) {
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

   for ( const std::pair< std::string, FunctionTestContent>& func : mockFuncTestCollection->dataFromJson() ) {

      const std::string &name = func.first;
      const FunctionTestContent &funcParams = func.second;
      const clang::FunctionDecl *funcDecl = funcParams.getFunctionDecl();
      int counter = 0;
      const std::vector< std::shared_ptr<FunctionTestData> >& tests = funcParams.getTests();
      const unsigned int size = funcParams.getNumTests();
      
      for (int i=0; i< size; ++i ) {
         const std::string namewithcounter = name + "_" + std::to_string(counter);
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
   Include["include"] = boost::filesystem::path(_fileName).filename().string() + "-mocks.h";
   context->add("includes", Include);
   Include["include"] = boost::filesystem::path(_fileName).filename().string() + "-serialization-struct.h";
   context->add("includes", Include);

   std::ostringstream    code;

  /*
   for (const std::pair< std::string, FunctionTestContent>&& iter : funcData->dataFromJson() ) {

      const FunctionTestContent& funcParams = iter.second;

      //       const clang::FunctionDecl* funcDecl = funcParams.getFunctionDecl();

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
   std::string TestFilename = boost::filesystem::path(_fileName).filename().string();
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

   for (const std::string& iter : includePaths) {
      Include["include"] = iter;
      context->add("includes", Include);
   }


   for (const auto& typedefDecl : typedefNameDecls) {
      objectToSerialize.clear();

      const clang::QualType typedefQualType = typedefDecl->getUnderlyingType(); // ->getCanonicalTypeInternal();
      // appends the row and column to the name string
      const std::string declSrcFile = utils::getDeclSourceFileLine(typedefDecl, _sourceMgr);

      objectToSerialize["name"] = typedefDecl->getNameAsString();
      objectToSerialize["file"] = declSrcFile;

      if (const clang::RecordType *structType = typedefQualType->getAsStructureType()) {
         const clang::RecordDecl *structDecl = structType->getDecl();

         objectToSerialize["typedefStructName"] = structDecl->getNameAsString();

         out.str("");
         for (const auto& field : structDecl->fields()) {
            out << "   " << field->getType().getAsString() << "\t" << field->getNameAsString() << ";\n";
         }

         objectToSerialize["fieldsToSerialize"] = out.str();
         objectToSerialize["typedefStructDeclName"] = typedefDecl->getNameAsString();

         context->add("structsToSerialize", objectToSerialize);

      } else if (const clang::EnumType *enumType = typedefQualType->getAs<clang::EnumType>()) {
         const clang::EnumDecl *enumDecl = enumType->getDecl();
         objectToSerialize["typedefEnumName"] = enumDecl->getNameAsString();

         out.str("");
         for (const auto& iter : enumDecl->enumerators()) {
            out << "   " << iter->getNameAsString() << "\t = " << iter->getInitVal().toString(10) << ",\n";
         }

         objectToSerialize["valuesToSerialize"] = out.str();

         const std::string qualTypeName = typedefQualType.getAsString();
         const clang::QualType canonicalQualType = typedefQualType->getCanonicalTypeInternal();
         const std::string canonicalTypeName = canonicalQualType.getAsString();

         objectToSerialize["typedefEnumDeclName"] = canonicalTypeName;

         context->add("enumsToSerialize", objectToSerialize);
      }  else if (const clang::TypedefType *typedefType = typedefQualType->getAs<clang::TypedefType>()) {

         const clang::QualType canonicalQualType = typedefType->getCanonicalTypeInternal();
         const std::string canonicalTypeName = canonicalQualType.getAsString();

         objectToSerialize["typedefType"] = typedefQualType.getAsString();
         objectToSerialize["typedefDeclName"] = typedefDecl->getNameAsString();

         context->add("typedefToSerialize", objectToSerialize);
      } else if (const clang::BuiltinType *typedefType = typedefQualType->getAs<clang::BuiltinType>()) {
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


   for (const std::string& iter : includePaths) {
      Include["include"] = iter;
      context->add("includes", Include);
   }
   //include
   Include["include"] = utils::changeFileExtension(boost::filesystem::path(_fileName).filename().string(), "h");
   context->add("includes", Include);

   //    context->add("testFilename", TestFilename);
   context->add("filename", _fileName);
   context->add("newline", "\n");

   // write function support structures
   for (const auto& iter : funcDeclsMap) {
      const clang::FunctionDecl *funcDecl = iter.first;
      // get declaration source location
      paramsStructsObject["functionName"] = funcDecl->getNameInfo().getName().getAsString();

      std::string returnType = funcDecl->getReturnType().getAsString();
      paramsStructsObject["returnType"] = returnType;

      out.str("");
      const int numParms = funcDecl->getNumParams();

      for (int i = 0; i < numParms; ++i) {
         const clang::ParmVarDecl *_currentParam = funcDecl->getParamDecl(i);
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


   for (const std::string& iter : includePaths) {
      Include["include"] = iter;
      context->add("includes", Include);
   }
   //Include["include"] = Include["include"] = utils::changeFileExtension(boost::filesystem::path(_fileName).filename().string(), "h");
   context->add("includes", Include);

   context->add("filename", _fileName);
   context->add("newline", "\n");

   /*
   std::vector<FunctionTestContent> funcParamsStructures;

   for (const std::pair<const clang::FunctionDecl *, FunctionDeclSet >& iter : funcDeclsMap) {
      const clang::FunctionDecl *funcDecl = iter.first;
      const std::set<const clang::FunctionDecl *> &mockDeclSet = iter.second;
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
   template_t     t;
   stringstream   buffer;
   string         result;

   boost::filesystem::path full_path(boost::filesystem::current_path());
   ifstream template_file(templateFileName);

   if (template_file.fail()) {
      std::cout << "template file for mock functions not found";
      return;
   }

   buffer << template_file.rdbuf();
   string         template_buff(buffer.str());

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
   }
   outputFile.close();

   
}


