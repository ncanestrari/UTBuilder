
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>

#include "writer.h"
#include "utils.h"
#include "Results.h"

#include "FuncParamsStructure.h"

#include <boost/filesystem/convenience.hpp>
#include <clang/AST/Decl.h>
#include <clang/AST/PrettyPrinter.h>
#include <plustache_types.hpp>
#include <template.hpp>

using std::cout;
using std::string;
using std::ifstream;
using std::stringstream;
using PlustacheTypes::ObjectType;
using PlustacheTypes::CollectionType;
using Plustache::Context;
using Plustache::template_t;


Writer::Writer( const std::string&            fileName,
	const clang::SourceManager&   sourceMgr)
: _fileName(fileName)
, _sourceMgr(sourceMgr)
{
}

void Writer::createExampleJsonFiles(void)
{
   CreateSerializationJsonfile( MockFunctionsData::get(), "mocks", _fileName + "-mocks-template");
   CreateSerializationJsonfile( UnitTestFunctionsData::get(), "funcs", _fileName + "-tests-template");
}


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
   for ( auto funcToMock : FunctionsToMock::get().declKeySetMap ) 
   {
      const clang::FunctionDecl* funcDecl = funcToMock.first;
      // get declaration source location
      const std::string declSrcFile = utils::getDeclSourceFile(funcDecl, _sourceMgr);
      
      includePaths.insert( boost::filesystem::path(declSrcFile).filename().string() );
   }

   //std::shared_ptr<const Plustache::Context> context = CreateMockContext(includePaths, results::get().functionDecls );

   WriteTemplate( CreateMockContext(includePaths, FunctionsToMock::get().declKeySetMap ),
		  std::string(std::getenv("TEMPLATE_DIR"))+std::string("/mock.template"), 
		  utils::changeFilePathToInclude(_fileName) + "-mocks.h");
   
}


void Writer::createUnitTestFile(void)
{

   std::string fnameUT = boost::filesystem::path(_fileName).filename().string();

   std::set<std::string> includePaths;

   // look for paths to include in the mock file
   for ( auto funcToUnitTest : FunctionsToUnitTest::get().declKeySetMap )
   {
      const clang::FunctionDecl* funcDecl = funcToUnitTest.first;
      // get declaration source location
      const std::string declSrcFile = utils::getDeclSourceFile(funcDecl, _sourceMgr);
      
      boost::filesystem::path p(declSrcFile);

      includePaths.insert( utils::changeFileExtension(p.filename().string(), "h") );
   }

   
   //std::shared_ptr<const Plustache::Context> context = CreateUnitTestContext(includePaths, results::get().functionsToUnitTest );
   
   WriteTemplate( CreateUnitTestContext(includePaths, UnitTestFunctionsData::get() ), 
		  std::string(std::getenv("TEMPLATE_DIR"))+std::string("/UT.template"), 
		  _fileName + "-ugtest.cpp" );
 
}


void Writer::CreateSerializationFile(void){
   std::ostringstream out;

   boost::filesystem::path fpathUT(_fileName);
   std::string fnameUT = fpathUT.filename().string();

   std::set<std::string> includePaths;

   for ( auto typedefDecl : results::get().typedefNameDecls )
   {
      // get declaration source location      
      const std::string declSrcFile = utils::getDeclSourceFile(typedefDecl, _sourceMgr);
      
      const std::string includeFile =  boost::filesystem::path(declSrcFile).filename().string();
      
      if ( !includeFile.empty() )
      {
         includePaths.insert( includeFile );
      }
   }

   for ( auto structDecl : results::get().structDecls )
   {
      // get declaration source location
      const std::string declSrcFile = utils::getDeclSourceFile(structDecl, _sourceMgr);
      
      const std::string includeFile =  boost::filesystem::path(declSrcFile).filename().string();
      
      if ( !includeFile.empty() )
      {
         includePaths.insert( includeFile );
      }
   }

   
   WriteTemplate( CreateSerializationContext(includePaths, results::get().typedefNameDecls ), 
                  std::string(std::getenv("TEMPLATE_DIR"))+std::string("/serialization.template"), 
                  _fileName + "-serialization.h" );

   /*
   WriteTemplate( CreateSerializationStructuresContext(includePaths, results::get().functionsToUnitTest ), 
                  std::string(std::getenv("TEMPLATE_DIR"))+std::string("/serialization-struct.template"), 
                  _fileName + "-serialization-struct.h" );
   */
   
   WriteTemplate( CreateStructuresToSerializeContext(includePaths, FunctionsToUnitTest::get().declKeySetMap ), 
                  std::string(std::getenv("TEMPLATE_DIR"))+std::string("/serialization-struct.template"), 
                  utils::changeFilePathToInclude(_fileName) + "-serialization-struct.h" );
   
   
   
}
         
         
void Writer::CreateSerializationJsonfile(const FunctionsData& functionData, 
                                         const std::string & objectName, 
                                         const std::string & outFileName )
{
//    std::vector<FuncParamsStruct> funcParamsStructures;
//    UnitTestFunctionsData::get().clear();
   
   Json::Value jsonRoot;
   
//    FuncParamsStruct funcParamsStruct;
   
//    if ( addMocks )
//    {
      
      // functions to mock
//       UnitTestFunctionsData::get().init(funcDeclMap);
      /*
      for ( auto funDecl : funcDeclMap) // results::get().functionsToMock )
      {
         const clang::FunctionDecl* decl = funDecl.first;
         const std::set<const clang::FunctionDecl*>& mockDeclSet = funDecl.second;
         funcParamsStruct.init( decl, mockDeclSet );
//          funcParamsStructures.push_back(funcParamsStruct);
         UnitTestFunctionsData::get().structs[funcParamsStruct.getName()] = funcParamsStruct;
      }
      */
//    }
//    else {
      
//       UnitTestFunctionsData::get().init(funcDeclMap, addMocks);
      /*
      for ( auto funDecl : funcDeclMap) // results::get().functionsToMock )
      {
         const clang::FunctionDecl* decl = funDecl.first;
         funcParamsStruct.init( decl );
//          funcParamsStructures.push_back(funcParamsStruct);
         UnitTestFunctionsData::get().structs[funcParamsStruct.getName()] = funcParamsStruct;
      }
      */
//    }
   
   
//    UnitTestFunctionsData::get().serialize(jsonRoot[objectName]);
   /*
   for (auto iter : UnitTestFunctionsData::get().structs )
   {
      iter.second.serialize(jsonRoot[objectName], addMocks );
   }
   */
   
   functionData.serializeAST(jsonRoot);
   
   std::ofstream outputFile;   
   std::string outputFileName = outFileName + ".json";   
   outputFile.open( outputFileName, std::fstream::out );
   outputFile << jsonRoot;
   outputFile.close();
   
   std::cout << "file written: " << outputFileName << std::endl;

   return;
}


std::shared_ptr<const Plustache::Context> Writer::CreateMockContext(const std::set<std::string>   &includePaths,
                                                                    const FunctionDeclKeySetMap   &funcDeclsMap ){
   
   std::shared_ptr<Plustache::Context> context = std::make_shared<Plustache::Context>();
   
   ObjectType            Include;
   ObjectType            Mock;
   ObjectType            FakeFunc;
   std::ostringstream    out;
   
   for(auto iter : includePaths){
      Include["include"] = iter;
      context->add("includes", Include);      
   }
   

   for ( auto iter : funcDeclsMap ) {
     
      const clang::FunctionDecl* funcDecl = iter.first;
      MockFunctionFFF( funcDecl, out );
      Mock["definition"] = out.str();
      Mock["mockname"] = funcDecl->getNameInfo().getName().getAsString();
      context->add("mocks", Mock);
      out.str("");
   }
   
   context->add("filename", _fileName);
   context->add("newline", "\n");
   
   
   out.str("");
   const std::map< std::string, FuncParamsStruct>&  dataJson = MockFunctionsData::get().dataJson();
   
   for ( auto func : dataJson ) {
    
      const std::string& name = func.first;
      const FuncParamsStruct& funcParams = func.second;
      const clang::FunctionDecl* funcDecl = funcParams.getFunctionDecl();
      int counter = 0;
      for ( auto iter : funcParams.getOutputTree()  ) {
	 const std::string namewithcounter = name + "_" + std::to_string(counter);
	 FakeFunctionDefinition(namewithcounter, funcDecl, iter, out);
	 FakeFunc["definition"] = out.str();
	 context->add("fakefuncs",FakeFunc);
	 counter++;
	 out.str("");
      }
      
   }
   
   
   return context;
}


std::shared_ptr<const Plustache::Context>
Writer::CreateUnitTestContext(const std::set<std::string>   &includePaths,
			      const UnitTestFunctionsData   &funcData ){
   
   std::shared_ptr<Plustache::Context> context = std::make_shared<Plustache::Context>();
   
   ObjectType   Include;
   ObjectType   FunctionToUnitTest;
   
   for(auto iter : includePaths){
      Include["include"] = iter;
      context->add("includes", Include);
   }
   // add mock file in includes list
   Include["include"] = boost::filesystem::path(_fileName).filename().string() + "-mocks.h";
   context->add("includes", Include);
   Include["include"] = boost::filesystem::path(_fileName).filename().string() + "-serialization-struct.h";
   context->add("includes", Include);

   
   std::ostringstream    code;
   
   for ( auto iter : UnitTestFunctionsData::get().dataJson() ) {
      
      FuncParamsStruct funcParams = iter.second;
      
//       const clang::FunctionDecl* funcDecl = funcParams.getFunctionDecl();
      
      const unsigned int size = funcParams.getSize();
      
      for (int i=0; i<size; ++i )
      {
         FunctionToUnitTest["functionName"] = funcParams.getName(i); // funcDecl->getNameAsString();
         
         code.str("");
         
         FuncParamsStruct::writeGoogleTest(code, funcParams, i );
        
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


std::shared_ptr<const Plustache::Context>
Writer::CreateSerializationContext(const std::set<std::string>                   &includePaths,
                                   const std::set<const clang::TypedefNameDecl*> &typedefNameDecls ) {

   std::shared_ptr<Plustache::Context> context = std::make_shared<Plustache::Context>();
                
   PlustacheTypes::ObjectType            Include;
   PlustacheTypes::ObjectType            objectToSerialize;

   
   std::ostringstream    out;
   
   context->add("filename", _fileName);
   context->add("newline", "\n");
   context->add("indent", "   ");
   
   for(auto iter : includePaths){
      Include["include"] = iter;
      context->add("includes", Include);      
   }
   

   for ( auto typedefDecl : typedefNameDecls  )
   {
      
      objectToSerialize.clear();
      
      const clang::QualType typedefQualType = typedefDecl->getUnderlyingType(); // ->getCanonicalTypeInternal();
      
      // appends the row and column to the name string
      const std::string declSrcFile = utils::getDeclSourceFileLine(typedefDecl, _sourceMgr);

      objectToSerialize["name"] = typedefDecl->getNameAsString();
      objectToSerialize["file"] = declSrcFile;         
     
         
      if( const clang::RecordType* structType = typedefQualType->getAsStructureType() ){

         
         const clang::RecordDecl* structDecl = structType->getDecl();
//          out << "typedef struct " << structDecl->getNameAsString() << "\n{\n";
      
         objectToSerialize["typedefStructName"] = structDecl->getNameAsString();
         
         out.str("");
         
         for ( const auto field : structDecl->fields() ){

            out << "   " << field->getType().getAsString() << "\t" << field->getNameAsString() << ";\n";
         }
      
         objectToSerialize["fieldsToSerialize"] = out.str();
         
         objectToSerialize["typedefStructDeclName"] = typedefDecl->getNameAsString();
      
//          out << "} " << typedefDecl->getNameAsString() << ";\n\n";

         //context->add( "fieldsToSerialize", structToSerialize);
         context->add( "structsToSerialize", objectToSerialize);
         
      }
      
      else if ( const clang::EnumType* enumType = typedefQualType->getAs<clang::EnumType>() ) {
   
         
         const clang::EnumDecl* enumDecl = enumType->getDecl();
         
//          out << "typedef enum " << enumDecl->getNameAsString()  << " {\n";
         objectToSerialize["typedefEnumName"] = enumDecl->getNameAsString();
         
         out.str("");
         
         for ( auto iter : enumDecl->enumerators() )
         {
            out << "   " << iter->getNameAsString() << "\t = " << iter->getInitVal().toString(10) << ",\n";
         }
         
         objectToSerialize["valuesToSerialize"] = out.str();
         
         const std::string qualTypeName = typedefQualType.getAsString();
         
         const clang::QualType canonicalQualType = typedefQualType->getCanonicalTypeInternal();
         const std::string canonicalTypeName = canonicalQualType.getAsString();
         
         objectToSerialize["typedefEnumDeclName"] = canonicalTypeName;
      
         context->add("enumsToSerialize", objectToSerialize);
//          out << "} " << canonicalTypeName << ";\n\n";         
      }
      
      else if ( const clang::TypedefType* typedefType = typedefQualType->getAs<clang::TypedefType>() ) {
         
//          const clang::TypedefNameDecl* typedefDecl = typedefType->getDecl();
         
         const clang::QualType canonicalQualType = typedefType->getCanonicalTypeInternal();
         const std::string canonicalTypeName = canonicalQualType.getAsString();
         
//          objectToSerialize["typedefType"] = canonicalTypeName;
         objectToSerialize["typedefType"] = typedefQualType.getAsString();
         objectToSerialize["typedefDeclName"] = typedefDecl->getNameAsString();
         
         context->add("typedefToSerialize", objectToSerialize);
         
      }

      else if ( const clang::BuiltinType* typedefType = typedefQualType->getAs<clang::BuiltinType>() ) {

         objectToSerialize["typedefType"] = typedefQualType.getAsString();
         objectToSerialize["typedefDeclName"] = typedefDecl->getNameAsString();
         
         context->add("typedefToSerialize", objectToSerialize);
      }
   }
   
   return context;
}



std::shared_ptr<const Plustache::Context>
Writer::CreateSerializationStructuresContext(const std::set<std::string> &includePaths,
                                             const FunctionDeclKeySetMap &funcDeclsMap ){
   
   std::shared_ptr<Plustache::Context> context = std::make_shared<Plustache::Context>();
   ObjectType            Include;
   ObjectType            paramsStructsObject;
   std::ostringstream    out;
 
   
   for(auto iter : includePaths){
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
   for ( auto iter : funcDeclsMap )
   {
      const clang::FunctionDecl* funcDecl = iter.first;
      // get declaration source location
//       const std::string declSrcFile = utils::getDeclSourceFile(funcDecl, _sourceMgr);
//       boost::filesystem::path p(declSrcFile);
//       includePaths.insert( utils::changeFileExtension(p.filename().string(), "h") );
      
      paramsStructsObject["functionName"] = funcDecl->getNameInfo().getName().getAsString();
      
      std::string returnType = funcDecl->getReturnType().getAsString();
      paramsStructsObject["returnType"] = returnType;
      
      out.str("");
      const int numParms = funcDecl->getNumParams();           

      for ( int i=0; i<numParms; ++i)
      {
         const clang::ParmVarDecl* _currentParam = funcDecl->getParamDecl(i);
         out << _currentParam->getType().getUnqualifiedType().getAsString() << "\t" << _currentParam->getQualifiedNameAsString() << ";\n   ";
         
         //out << _currentParam->getType().getAsString() << "\t" << _currentParam->getQualifiedNameAsString() << ";\n   "; 
      }
   
      paramsStructsObject["paramTypesAndNames"] = out.str();
      context->add("functionParamsStructs", paramsStructsObject);
   }   
   
   return context;
}

std::shared_ptr<const Plustache::Context>
Writer::CreateStructuresToSerializeContext(const std::set<std::string>   &includePaths,
                                           const FunctionDeclKeySetMap   &funcDeclsMap ){
   
   std::shared_ptr<Plustache::Context> context = std::make_shared<Plustache::Context>();
   
   ObjectType            Include;
   ObjectType            paramsStructsObject;
   std::ostringstream    out;
 
   
   for(auto iter : includePaths){
      Include["include"] = iter;
      context->add("includes", Include);
   }
   Include["include"] = Include["include"] = utils::changeFileExtension(boost::filesystem::path(_fileName).filename().string(), "h");
   context->add("includes", Include);
   
//    context->add("testFilename", TestFilename);   
   context->add("filename", _fileName);
   context->add("newline", "\n");
   
   
   
   // fill the FuncParamsStruct vector
   std::vector<FuncParamsStruct> funcParamsStructures;
   
   
   FuncParamsStruct funcParamsStruct;
   
   for ( auto iter : funcDeclsMap )
   {
      const clang::FunctionDecl* funcDecl = iter.first;
      
      const std::set<const clang::FunctionDecl*>& mockDeclSet = iter.second;
      
      funcParamsStruct.init( funcDecl, mockDeclSet );
      
      funcParamsStructures.push_back(funcParamsStruct);
//       UnitTestFunctionsData::get().structs[funcParamsStruct.getName()] = funcParamsStruct;
   }
   
   
   
   for (auto iter : funcParamsStructures )
   {      
      out.str("");
      FuncParamsStruct::writeAsStructure(out, iter);
     
      paramsStructsObject["functionName"] = iter.getName();
      paramsStructsObject["paramTypesAndNames"] = out.str();
      context->add("functionParamsStructs", paramsStructsObject);
   }   
   

   return context;
}

void
Writer::WriteTemplate(std::shared_ptr<const Plustache::Context>  context,
                      const std::string                         &templateFileName,
                      const std::string                         &outFileName){
   template_t     t;
   stringstream   buffer;
   string         result;
   
   boost::filesystem::path full_path( boost::filesystem::current_path() );
   
   std::cout << full_path.string();
   
   ifstream template_file(templateFileName);
   
   if (template_file.fail() )
   {
      std::cout << "template file for mock functions not found";
      return;
   }
   
   buffer << template_file.rdbuf();
   string         template_buff(buffer.str());
   
   result = t.render(template_buff, *context);
  
   std::ofstream outputFile;   
   std::string outputFileName = outFileName;   
   outputFile.open( outputFileName, std::fstream::out );
   outputFile << result;
   outputFile.close();
  
   std::cout << "file written: " << outputFileName << std::endl;
}


static void writeMockValue(std::ostringstream& os, 
                           const std::shared_ptr<NameValueTypeNode<clang::QualType> > tree, 
                           const std::string& name)
{
   
   std::string structName = name + tree->getName();

   
   if ( tree->getNumChildern() > 0 )
   {      
      
      structName += ".";
      
      for ( auto child : tree->getChildren() )
      {
         writeMockValue(os, child.second, structName);
      }
   }
   else{//TODO manage pointer to structure if needed

      
      if( tree->getValue() != "" ){
	 if( tree->getName() == "retval" ){
	    os << "   retval = " << tree->getValue() << ";\n";
	 } else if( tree->getType()->isAnyPointerType() ){
	    os << "   *" << structName << " = " << tree->getValue() << ";\n";
	 } else {
	    os << "   " << structName << " = " << tree->getValue() << ";\n";
	 }
      }
   }
   

}


void Writer::FakeFunctionDefinition(const std::string                                          &name,
				    const clang::FunctionDecl                                  *funcDecl,
				    const std::shared_ptr<NameValueTypeNode<clang::QualType> >  outTree,
				    std::ostringstream                                         &out)
{

   std::string returnType = funcDecl->getReturnType().getAsString();

   std::string isVariadic;

   out << returnType << " ";
   out << name << "(";
       
   const int numParms = funcDecl->getNumParams();           
   if (numParms == 0){
      out << "void";
   } else {
      const clang::ParmVarDecl* _currentParam = funcDecl->getParamDecl(0);
      out << _currentParam->getType().getAsString() << " " << _currentParam->getNameAsString();
      for ( int i=1; i<numParms; ++i)
      {
	 const clang::ParmVarDecl* _currentParam = funcDecl->getParamDecl(i);
	 out << ", " << _currentParam->getType().getAsString() << " " << _currentParam->getNameAsString();
      }

      if ( funcDecl->isVariadic() )
	 out << ", ...";
   }
   
   out <<" ){";
   
   out << "// fill the input struct with json file values" << "\n";
   
   
   for ( auto child : outTree->getChildren() )
   {
      if( child.first == "retval" ){
	 out << "   " << child.second->getType().getAsString() << " retval;\n";
	 out << "   memset(&retval,0,sizeof(" << child.second->getType().getAsString() << "));\n";
      }
      writeMockValue(out, child.second, "");
   }
   
//    for( auto child : obj._outputTree[i]->getChildren() ){
//       if(child.first=="retval"){
//          writeMockValue( os, child.second, "" );
//       } else {
//          writeMockValue( os, child.second, "input." );
//       }
//    }
//    
   
   if(returnType != "void"){
      out << "   return retval;\n";
   }
   
   out << "}";
}




void Writer::MockFunctionFFF(const clang::FunctionDecl   *funcDecl,
                             std::ostringstream          &out ){
   
   // get declaration source location
   // appends the row and column to the name string
   const std::string declSrcFile = utils::getDeclSourceFileLine(funcDecl, _sourceMgr);
   
   
   out << "/**" <<std::endl;
   out << " * name: " << funcDecl->getNameInfo().getName().getAsString() << std::endl;
   out << " * file: " << declSrcFile << std::endl;
   out << " */" << std::endl;   
   
   std::string returnType = funcDecl->getReturnType().getAsString();

   std::string isVariadic;

   if ( funcDecl->isVariadic() )
      isVariadic = "_VARARG";

   if ( returnType == "void" ){
      out << "FAKE_VOID_FUNC" << isVariadic << "( ";
   } else {
      out << "FAKE_VALUE_FUNC" << isVariadic << "( " << returnType << ", ";
   }
   

   out << funcDecl->getNameInfo().getName().getAsString();
       
   const int numParms = funcDecl->getNumParams();           

   for ( int i=0; i<numParms; ++i)
   {
      const clang::ParmVarDecl* _currentParam = funcDecl->getParamDecl(i);
      out << ", " << _currentParam->getType().getAsString(); 
   }

   if ( funcDecl->isVariadic() )
      out << ", ...";

   out <<" );";

   out << std::endl;

}




