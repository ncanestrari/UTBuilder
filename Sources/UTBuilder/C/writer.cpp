
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>

#include "writer.h"
#include "utils.h"
#include "Results.h"

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

void Writer::createFiles(void)
{
   CreateMockFile();
   CreateUnitTestFile();
   CreateSerializationFile();
}


void Writer::CreateMockFile(void)
{

   std::set<std::string> includePaths;

   // look for paths to include in the mock file
   for ( auto funcDecl : results::get().functionsToMock )
   {
      // get declaration source location
      const std::string declSrcFile = utils::getDeclSourceFile(funcDecl, _sourceMgr);
      
      includePaths.insert( boost::filesystem::path(declSrcFile).filename().string() );
   }

   //std::shared_ptr<const Plustache::Context> context = CreateMockContext(includePaths, results::get().functionDecls );

   WriteTemplate( CreateMockContext(includePaths, results::get().functionsToMock ),
		  std::string(std::getenv("TEMPLATE_DIR"))+std::string("/mock.template"), 
		  _fileName + "-mocks.h");
   
}


void Writer::CreateUnitTestFile(void)
{

   std::string fnameUT = boost::filesystem::path(_fileName).filename().string();

   std::set<std::string> includePaths;

   // look for paths to include in the mock file
   for ( auto funcDecl : results::get().functionsToUnitTest )
   {
      // get declaration source location
      const std::string declSrcFile = utils::getDeclSourceFile(funcDecl, _sourceMgr);
      
      boost::filesystem::path p(declSrcFile);

      includePaths.insert( utils::changeFileExtension(p.filename().string(), "h") );
   }

   
   //std::shared_ptr<const Plustache::Context> context = CreateUnitTestContext(includePaths, results::get().functionsToUnitTest );
   
   WriteTemplate( CreateUnitTestContext(includePaths, results::get().functionsToUnitTest ), 
		  std::string(std::getenv("TEMPLATE_DIR"))+std::string("/UT.template"), 
		  _fileName + "-ugtest.c" );
 
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

   
   WriteTemplate( CreateSerializationStructuresContext(includePaths, results::get().functionsToUnitTest ), 
                  std::string(std::getenv("TEMPLATE_DIR"))+std::string("/serialization-struct.template"), 
                  _fileName + "-serialization-struct.h" );
   
}


std::shared_ptr<const Plustache::Context> Writer::CreateMockContext(const std::set<std::string>                  &includePaths,
                                                                    const std::set<const clang::FunctionDecl*>   &funcDecls ){
   
   std::shared_ptr<Plustache::Context> context = std::make_shared<Plustache::Context>();
   
   ObjectType            Include;
   ObjectType            Mock;
   std::ostringstream    out;
   
   for(auto iter : includePaths){
      Include["include"] = iter;
      context->add("includes", Include);      
   }
   

   for ( auto iter : funcDecls ) {
     
      MockFunctionFFF( iter, out );
      Mock["definition"] = out.str();
      context->add("mocks", Mock);
      out.str("");
   }
   
   context->add("filename", _fileName);
   context->add("newline", "\n");
   
   return context;
}


std::shared_ptr<const Plustache::Context> Writer::CreateUnitTestContext(const std::set<std::string>                  &includePaths,
									const std::set<const clang::FunctionDecl*>   &functionToUnitTest ){
   
   std::shared_ptr<Plustache::Context> context = std::make_shared<Plustache::Context>();
   
   ObjectType            Include;
   ObjectType            FunctionToUnitTest;
   
   for(auto iter : includePaths){
      Include["include"] = iter;
      context->add("includes", Include);      
   }
   // add mock file in includes list
   Include["include"] = boost::filesystem::path(_fileName).filename().string() + "-mocks.h";
   context->add("includes", Include); 

   for ( auto iter : functionToUnitTest ) {
      FunctionToUnitTest["functionName"] = iter->getNameAsString();
      context->add("functionToUnitTest", FunctionToUnitTest);
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


std::shared_ptr<const Plustache::Context> Writer::CreateSerializationContext(
   const std::set<std::string>                  &includePaths, 
   const std::set<const clang::TypedefNameDecl*>&   typedefNameDecls ) {

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
         
         const clang::TypedefNameDecl* typedefDecl = typedefType->getDecl();
         
         const clang::QualType canonicalQualType = typedefType->getCanonicalTypeInternal();
         const std::string canonicalTypeName = canonicalQualType.getAsString();
         
         objectToSerialize["typedefType"] = canonicalTypeName;
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



std::shared_ptr<const Plustache::Context> Writer::CreateSerializationStructuresContext(const std::set<std::string>& includePaths,
                                                                        const std::set<const clang::FunctionDecl*>& functionToUnitTest ){
   
   std::shared_ptr<Plustache::Context> context = std::make_shared<Plustache::Context>();
   
   ObjectType            Include;
   ObjectType            paramsStructsObject;
   std::ostringstream    out;
 
   
   for(auto iter : includePaths){
      Include["include"] = iter;
      context->add("includes", Include);      
   }

   
//    context->add("testFilename", TestFilename);   
   context->add("filename", _fileName);
   context->add("newline", "\n");
   
   
   
   // write function support structures
   for ( auto funcDecl : results::get().functionsToUnitTest )
   {
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

void Writer::WriteTemplate(std::shared_ptr<const Plustache::Context>      context,
                                 const std::string& templateFileName,
                                 const std::string                      &outFileName){
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




