#include "Writers.h"


#include <plustache_types.hpp>
#include <sstream>
#include <string>
#include <template.hpp>

#include "UnitTestDataUtils.h"
#include "Utils.h"


using std::cout;
using std::string;
using std::ifstream;
using std::stringstream;
using PlustacheTypes::ObjectType;
using PlustacheTypes::CollectionType;
using Plustache::Context;
using Plustache::template_t;



const std::string MockWriter::_templateName = BaseWriter::_templateDir + std::string("/mock.template");
const std::string MockWriter::_templateSuffix = std::string("-mocks.h");
    
const std::string SerializationWriter::_templateName = BaseWriter::_templateDir + std::string("/serialization.template");
const std::string SerializationWriter::_templateSuffix = std::string("-serialization.h");

const std::string StructuresToSerializeWriter::_templateName = BaseWriter::_templateDir + std::string("/serialization-struct.template");
const std::string StructuresToSerializeWriter::_templateSuffix = std::string("-serialization-struct.h");

const std::string UnitTestFileWriter::_templateName = BaseWriter::_templateDir + std::string("/UT.template");
const std::string UnitTestFileWriter::_templateSuffix = std::string("-ugtest.cpp");
    
const std::string CMakeFileWriter::_templateName = BaseWriter::_templateDir + std::string("/CMakeFile.template");
const std::string CMakeFileWriter::_templateSuffix = std::string("");

const std::string GlobalsWriter::_templateName = BaseWriter::_templateDir + std::string("/globals.template");
const std::string GlobalsWriter::_templateSuffix = std::string("-globals.h");



MockWriter::MockWriter() = default;
   

const Plustache::Context* MockWriter::createContext() 
{
   std::set<std::string> includePaths;

   // look for paths to include in the mock file
   const FunctionDeclKeySetMap& funcsToMock = *_data->getFunctionToMock();
   for (const auto& funcToMock : funcsToMock ) {
      
      const clang::FunctionDecl *funcDecl = funcToMock.first;
      // get declaration source location
      const std::string declSrcFile = Utils::getDeclSourceFile(funcDecl, *_sourceMgr);

      includePaths.insert(boost::filesystem::path(declSrcFile).filename().string());
   }
   
   
   
   
   Plustache::Context* context = new Plustache::Context();
   
   ObjectType            Include;
   ObjectType            Mock;
   ObjectType            FakeFunc;
   std::ostringstream    out;

   for (const std::string& iter : includePaths) {
      Include["include"] = iter;
      context->add("includes", Include);
   }

   for (const auto& iter : funcsToMock) {

      const clang::FunctionDecl *funcDecl = iter.first;
      UnitTestDataUtils::writeMockFunctionFFF(funcDecl, _sourceMgr, out);
      Mock["definition"] = out.str();
      Mock["mockname"] = funcDecl->getNameInfo().getName().getAsString();
      context->add("mocks", Mock);
      out.str("");
   }

   context->add("filename", _fileName);
   context->add("newline", "\n");

   out.str("");


   const NameValueNode* mocksdata = _data->getMocksData();
   const std::map< std::string, std::unique_ptr<NameValueNode> >& mockFuncs = mocksdata->getChildren();
   for (const auto& arrayIndex : mockFuncs) {
      
      const NameValueNode* childObj = arrayIndex.second.get();
      const FunctionDeclNode* funcName = dynamic_cast<const FunctionDeclNode*>(childObj->getChild("_name"));
      
      if ( funcName == nullptr ){
         std::cout << "PROBLEM: no ""_name"" field for mocks array index " << childObj->getName() << std::endl;
         continue;
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



SerializationWriter::SerializationWriter() = default;

   
const Plustache::Context* SerializationWriter::createContext() 
{
   boost::filesystem::path fpathUT(_fileName);
   std::string fnameUT = fpathUT.filename().string();
   std::set<std::string> includePaths;

      
   for (const auto& typedefDecl : _info->getTypedefNameDecls() ) {
      // get declaration source location
      const std::string declSrcFile = Utils::getDeclSourceFile(typedefDecl, *_sourceMgr);
      const std::string includeFile =  boost::filesystem::path(declSrcFile).filename().string();
      if (!includeFile.empty()) {
         includePaths.insert(includeFile);
      }
   }

   for (const auto& structDecl : _info->getStructureDecls() ) {
      // get declaration source location
      const std::string declSrcFile = Utils::getDeclSourceFile(structDecl, *_sourceMgr);
      const std::string includeFile =  boost::filesystem::path(declSrcFile).filename().string();
      if (!includeFile.empty()) {
         includePaths.insert(includeFile);
      }
   }
   
   
   
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

   
   for (const auto& typedefDecl : _info->getTypedefNameDecls() ) {
      objectToSerialize.clear();

      const clang::QualType typedefQualType = typedefDecl->getUnderlyingType(); // ->getCanonicalTypeInternal();
      // appends the row and column to the name string
      const std::string declSrcFile = Utils::getDeclSourceFileLine(typedefDecl, *_sourceMgr);

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


StructuresToSerializeWriter::StructuresToSerializeWriter() = default;
   
const Plustache::Context* StructuresToSerializeWriter::createContext() 
{
   boost::filesystem::path fpathUT(_fileName);
   std::string fnameUT = fpathUT.filename().string();
   std::set<std::string> includePaths;

   
   for (const auto& typedefDecl : _info->getTypedefNameDecls() ) {
      // get declaration source location
      const std::string declSrcFile = Utils::getDeclSourceFile(typedefDecl, *_sourceMgr);
      const std::string includeFile =  boost::filesystem::path(declSrcFile).filename().string();
      if (!includeFile.empty()) {
         includePaths.insert(includeFile);
      }
   }

   for (const auto& structDecl : _info->getStructureDecls() ) {
      // get declaration source location
      const std::string declSrcFile = Utils::getDeclSourceFile(structDecl, *_sourceMgr);
      const std::string includeFile =  boost::filesystem::path(declSrcFile).filename().string();
      if (!includeFile.empty()) {
         includePaths.insert(includeFile);
      }
   }
   
   
   Plustache::Context* context = new Plustache::Context();
   
   ObjectType            Include;
   ObjectType            paramsStructsObject;
   std::ostringstream    out;


   for (const std::string& iter : includePaths) {
      Include["include"] = iter;
      context->add("includes", Include);
   }
   context->add("includes", Include);

   context->add("filename", _fileName);
   context->add("newline", "\n");

   
   const FunctionDeclKeySetMap&  funcDeclsMap = *_data->getFunctionToTest();
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
   
   
UnitTestFileWriter::UnitTestFileWriter() = default;
   
   
const Plustache::Context* UnitTestFileWriter::createContext() 
{
   std::string fnameUT = boost::filesystem::path(_fileName).filename().string();

   std::set<std::string> includePaths;

   // look for paths to include in the mock file
   const FunctionDeclKeySetMap&  funcDeclsMap = *_data->getFunctionToTest();
   for (const auto& funcToUnitTest : funcDeclsMap ) { /*FunctionsToUnitTest::get().declKeySetMap) {*/
      const clang::FunctionDecl *funcDecl = funcToUnitTest.first;
      // get declaration source location
      const std::string declSrcFile = Utils::getDeclSourceFile(funcDecl, *_sourceMgr);
      boost::filesystem::path p(declSrcFile);
      includePaths.insert(Utils::changeFileExtension(p.filename().string(), "h"));
   }
   
   
   
   Plustache::Context* context = new Plustache::Context();
   
   ObjectType   Include;
   ObjectType   FunctionToUnitTest;

   for (const std::string& iter : includePaths) {
      Include["include"] = iter;
      context->add("includes", Include);
   }
   // add mock file in includes list
   Include["include"] = boost::filesystem::path(_fileName).filename().string() + MockWriter::_templateSuffix;
   context->add("includes", Include);
   Include["include"] = boost::filesystem::path(_fileName).filename().string() + StructuresToSerializeWriter::_templateSuffix;
   context->add("includes", Include);
   Include["include"] = boost::filesystem::path(_fileName).filename().string() + GlobalsWriter::_templateSuffix;
   context->add("includes", Include);
   
   std::ostringstream    code;
   
   const NameValueNode* funcsToTest = _data->getFuncsData();
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
   TestFilename = Utils::removeDashes(TestFilename);
   TestFilename = Utils::removeFileExtension(TestFilename);

   context->add("testFilename", TestFilename);
   context->add("filename", _fileName);
   context->add("newline", "\n");

   return context; 
}



CMakeFileWriter::CMakeFileWriter() = default;


void CMakeFileWriter::init(const std::string&    fileName,
                           const UnitTestData*   data,
                           const ClangCompiler*  compiler )
{
   
   //    _compiler = compiler;
   _info = &compiler->getASTinfo();
   _sourceMgr = &compiler->getSourceManager();
   
   _testFileName = boost::filesystem::path(fileName).filename().string() + UnitTestFileWriter::_templateSuffix;
 
   const std::string outputFileName = compiler->getProjectDescription().getOutputFileName();
   _fileName = "CMakeLists.txt";
   _data = data;
  
   _sources = &compiler->getProjectDescription().getSources();
   _includePaths = &compiler->getProjectDescription().getIncludePaths();
   _workspace = &compiler->getProjectDescription().getWorkspace();
}


const Plustache::Context* CMakeFileWriter::createContext() 
{
   Plustache::Context* context = new Plustache::Context();
   PlustacheTypes::ObjectType   objType;

   for (const boost::filesystem::path& iter : *_includePaths) {
      objType["include"] = (*_workspace / iter).string();
      context->add("includes", objType);
   }

   objType.clear();
   
   for (const boost::filesystem::path& iter : *_sources) {
      objType["source"] = (*_workspace / iter).string();
      context->add("sources", objType);
   }

   std::string projectName = Utils::changeFileExtension(boost::filesystem::path(_testFileName).filename().string(), "");
   
   context->add("ProjectName", projectName);
   context->add("TestFileName", _testFileName);
   context->add("newline", "\n");
  
   return context; 
}


GlobalsWriter::GlobalsWriter() = default;
   

const Plustache::Context* GlobalsWriter::createContext() 
{   
   const NameValueNode* globalsNode = _data->getGlobalsData();
   
   std::set<std::string> includePaths;
   
   const std::map< std::string, std::unique_ptr<NameValueNode> >&  globalsChildren = globalsNode->getChildren();
   for (const auto& globalIter : globalsChildren ) {
      const NameValueNode* typeNode = globalIter.second->getChild("_type");
      const clang::TypedefNameDecl * typeDefName = _info->getAllTypesMap().find(typeNode->getValue())->second;
      //It MUST be there
      const std::string declSrcFile = Utils::getDeclSourceFile(typeDefName, *_sourceMgr);
      includePaths.insert(boost::filesystem::path(declSrcFile).filename().string());
//      includePaths.insert( Utils::getDeclSourceFileLine( typeDefName, *_sourceMgr) );
   }
   
   Plustache::Context* context = new Plustache::Context();

   ObjectType   Include;
   ObjectType   GlobalToSet;

   for (const std::string& iter : includePaths) {
      Include["include"] = iter;
      context->add("includes", Include);
   }   
   
   std::ostringstream    out;


   for (const auto& globalIter : globalsChildren) {
      std::string globalName = globalIter.second->getChild("_name")->getValue();
      std::string globalType = globalIter.second->getChild("_type")->getValue();
      const NameValueNode* contentNode = globalIter.second->getChild("content");
      const std::map< std::string, std::unique_ptr<NameValueNode> >&  contentChildren = contentNode->getChildren();
      std::string globalSize = std::to_string( contentChildren.size() );
      for (const auto& contentIter : contentChildren ) {
         UnitTestDataUtils::writeGlobalsContent(out, globalName, contentIter.second.get());
      }
      
      GlobalToSet["name"] = globalName;
      GlobalToSet["type"] = globalType;
      GlobalToSet["size"] = globalSize;
      
      GlobalToSet["definition"] = out.str();
      context->add("globals", GlobalToSet);

      out.str("");
   }
   context->add("newline", "\n");
   return context;   
}




