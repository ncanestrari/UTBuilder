#include "utils.h"
#include "globber.h"

#include <clang/AST/Decl.h>
#include <boost/filesystem/convenience.hpp>

using std::set;
using std::string;
using std::toupper;
using std::vector;


string utils::extractCommercialCodePath(const string& fileNamePath){
   size_t lastdot = fileNamePath.find("/CommercialCode/");
   return fileNamePath.substr(0, lastdot) + "/CommercialCode/";
}


string utils::extractPackagePath(const string& fileNamePath){
   string cc("/CommercialCode/");
   size_t lastdot = fileNamePath.find(cc);
   string rest = fileNamePath.substr(lastdot+cc.length(), fileNamePath.length()-1);
   lastdot = rest.find("/");   
   return rest.substr(0, lastdot);
}


vector<string>* utils::getIncludePaths(const string& package){
   vector<string> *include_folders = new vector<string>(0);
   string repo(package);
   //now str is the Repository
   const string matchers_[] = { "/Sources/*/Include/", "/Input_API/*/API/", "/Input_API/*/API/*/" };
   const vector<string> matchers(matchers_, matchers_ + sizeof(matchers_)/sizeof(string) );
   for( const auto matcher : matchers ){
      string path = repo + matcher;
      vector<string> ret = glob(path);
      include_folders->insert(include_folders->end(), ret.begin(), ret.end());
   }
   
   for( string &str : *include_folders )
      str.insert(0,"-I");

   return include_folders;
}


string utils::removeFileExtension(const string& fileNamePath){
   size_t lastdot = fileNamePath.find_last_of(".");
   return (lastdot == string::npos) ? fileNamePath : fileNamePath.substr(0, lastdot);
}


string utils::changeFileExtension(const string& fileName, const string& newExt){
   return boost::filesystem::change_extension(fileName, newExt).string();
}


string utils::removeDashes( const string& fileNamePath){
   string fileName = boost::filesystem::path(fileNamePath).filename().string();
   
   if (fileName.empty() )
      return fileName;
   
   size_t pos = 0;
   fileName[pos] = toupper(fileName[pos]); 
   
   pos = fileName.find("-", pos);
   while ( pos !=  string::npos ){
      fileName.erase(pos, 1);
      fileName[pos] = toupper(fileName[pos]);   
      pos = fileName.find("-", pos);
   }
   
   return boost::filesystem::path(fileName).parent_path().string() + fileName;  
}


void utils::fillFunctionQualTypes(void){
   results::get().functionDeclTypes.clear();
   
   //canonical Types: http://clang.llvm.org/docs/InternalsManual.html#canonical-types
   for ( const clang::FunctionDecl* funcDecl : results::get().functionDecls ){
      const clang::QualType returnType = funcDecl->getReturnType();
      results::get().functionDeclTypes.insert( returnType->getCanonicalTypeInternal().getTypePtrOrNull() );
      
      const int numParms = funcDecl->getNumParams();   
      for ( int i=0; i<numParms; ++i){
         const clang::ParmVarDecl* _currentParam = funcDecl->getParamDecl(i);
         const clang::QualType qualType = _currentParam->getOriginalType();
         results::get().functionDeclTypes.insert( qualType->getCanonicalTypeInternal().getTypePtrOrNull() ); 
      }
   }
   
   for ( const clang::FunctionDecl* funcDecl : results::get().functionToUnitTest ){
      const clang::QualType returnType = funcDecl->getReturnType();
      results::get().functionDeclTypes.insert( returnType->getCanonicalTypeInternal().getTypePtrOrNull() );
         
      const int numParms = funcDecl->getNumParams();   
      for ( int i=0; i<numParms; ++i){
         const clang::ParmVarDecl* _currentParam = funcDecl->getParamDecl(i);   
         const clang::QualType qualType = _currentParam->getOriginalType();
         
         results::get().functionDeclTypes.insert( qualType->getCanonicalTypeInternal().getTypePtrOrNull() );
      }
   }
}

