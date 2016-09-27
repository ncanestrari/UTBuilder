#ifndef _UTBuilder_utils_h__
#define _UTBuilder_utils_h__

#include <string>
#include <set>
#include <vector>

#include <clang/Basic/SourceManager.h>

namespace clang {
  class Decl;
  class Stmt;
}


class utils 
{

public:
   

   static std::string extractCommercialCodePath(const std::string &fileNamePath);
   static std::string extractPackagePath(const std::string &fileNamePath);
   static std::string removeFileExtension(const std::string &fileName);
   static std::string changeFilePathToInclude(const std::string& fileName);
   static std::string changeFileExtension(const std::string &fileName,
                                          const std::string &newExtension);
   static std::string removeDashes(const std::string &fileNamePath);
   static void getIncludePaths(const std::string        &package,
                               std::vector<std::string> &include_folders);
   static void fillFunctionQualTypes(void);
   static std::string getDeclSourceFile( const clang::Decl* decl, const clang::SourceManager& srcMgr );
   
   // append the row and column to the file name name string 
   static std::string getDeclSourceFileLine(const clang::Decl *decl,
                                            const clang::SourceManager &srcMgr);   
   static std::string getStmtSourceFile(const clang::Stmt           *stmt,
                                        const clang::SourceManager  &srcMgr);
   // append the row and column to the file name name string 
   static std::string getStmtSourceFileLine(const clang::Stmt          *stmt,
                                            const clang::SourceManager &srcMgr);
   
};


#endif // _UTBuilder_utils_h__

