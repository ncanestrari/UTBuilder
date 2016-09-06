#ifndef _UTBuilder_utils_h__
#define _UTBuilder_utils_h__

#include <string>

#include <clang/Basic/SourceManager.h>

namespace clang {
  class Decl;
  class Stmt;
}


class utils 
{

public:
   
   static std::string removeFileExtension(const std::string& fileName);

   static std::string changeFileExtension(const std::string& fileName, const std::string& newExtension);

   static std::string removeDashes( const std::string& fileNamePath);
   
   static void fillFunctionQualTypes(void);
   
   
   static const char* getDeclSourceFile( const clang::Decl* decl, const clang::SourceManager& srcMgr );
   
   // append the row and column to the file name name string 
   static const char* getDeclSourceFileLine( const clang::Decl* decl, const clang::SourceManager& srcMgr );
   
   static const char* getStmtSourceFile( const clang::Stmt* stmt, const clang::SourceManager& srcMgr );
   
   // append the row and column to the file name name string 
   static const char* getStmtSourceFileLine( const clang::Stmt* stmt, const clang::SourceManager& srcMgr );
   
};


#endif // _UTBuilder_utils_h__

