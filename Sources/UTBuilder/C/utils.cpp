#include "utils.h"

#include "Results.h"

#include <clang/AST/Decl.h>
#include <clang/AST/Stmt.h>

#include <boost/filesystem/convenience.hpp>


std::string utils::removeFileExtension( const std::string& fileNamePath)
{
   size_t lastdot = fileNamePath.find_last_of(".");
   
   if (lastdot == std::string::npos )
      return fileNamePath;
   else
      return fileNamePath.substr(0, lastdot);
  
}


std::string utils::changeFileExtension(const std::string& fileName, const std::string& newExt)
{
   return boost::filesystem::change_extension(fileName, newExt).string();
}


std::string utils::removeDashes( const std::string& fileNamePath)
{
   std::string fileName = boost::filesystem::path(fileNamePath).filename().string();
   
   if (fileName.empty() )
      return fileName;
   
   size_t pos = 0;
   fileName[pos] = std::toupper(fileName[pos]); 
   
   pos = fileName.find("-", pos);
   while ( pos !=  std::string::npos )
   {
      fileName.erase(pos, 1);
      fileName[pos] = std::toupper(fileName[pos]);   
      pos = fileName.find("-", pos);
   }
   
   return boost::filesystem::path(fileName).parent_path().string() + fileName;  
}


void utils::fillFunctionQualTypes(void)
{
   results::get().functionDeclTypes.clear();
   
   //canonical Types: http://clang.llvm.org/docs/InternalsManual.html#canonical-types
   
   for ( const clang::FunctionDecl* funcDecl : results::get().functionsToMock )
   {
      const clang::QualType returnType = funcDecl->getReturnType();
      
      results::get().functionDeclTypes.insert( returnType->getCanonicalTypeInternal().getTypePtrOrNull() );

      
      const int numParms = funcDecl->getNumParams();   
      for ( int i=0; i<numParms; ++i)
      {
         const clang::ParmVarDecl* _currentParam = funcDecl->getParamDecl(i);
         const clang::QualType qualType = _currentParam->getOriginalType();
         
         results::get().functionDeclTypes.insert( qualType->getCanonicalTypeInternal().getTypePtrOrNull() ); 
      }
   }
   
   
   for ( const clang::FunctionDecl* funcDecl : results::get().functionsToUnitTest )
   {
      const clang::QualType returnType = funcDecl->getReturnType();
      
      results::get().functionDeclTypes.insert( returnType->getCanonicalTypeInternal().getTypePtrOrNull() );
         
      const int numParms = funcDecl->getNumParams();   
      for ( int i=0; i<numParms; ++i)
      {
         const clang::ParmVarDecl* _currentParam = funcDecl->getParamDecl(i);   
         const clang::QualType qualType = _currentParam->getOriginalType();
         
         results::get().functionDeclTypes.insert( qualType->getCanonicalTypeInternal().getTypePtrOrNull() );
      }
   }
   
}


const char* utils::getDeclSourceFile( const clang::Decl* decl, const clang::SourceManager& srcMgr )
{
  // source location
   const clang::SourceLocation srcLoc = decl->getSourceRange().getBegin();
   return srcMgr.getFilename(srcLoc).str().c_str();
}

const char* utils::getDeclSourceFileLine( const clang::Decl* decl, const clang::SourceManager& srcMgr )
{
   // source location
   const clang::SourceLocation srcLoc = decl->getSourceRange().getBegin();
   return srcLoc.printToString(srcMgr).c_str();
}


const char* utils::getStmtSourceFile( const clang::Stmt* stmt, const clang::SourceManager& srcMgr )
{
  // source location
   const clang::SourceLocation srcLoc = stmt->getSourceRange().getBegin();
   return srcMgr.getFilename(srcLoc).str().c_str();
}

const char* utils::getStmtSourceFileLine( const clang::Stmt* stmt, const clang::SourceManager& srcMgr )
{
   // source location
   const clang::SourceLocation srcLoc = stmt->getSourceRange().getBegin();
   return srcLoc.printToString(srcMgr).c_str();
}
