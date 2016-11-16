
#ifndef _UTBuilder_Utils_h__
#define _UTBuilder_Utils_h__

#include <boost/filesystem.hpp>
#include <clang/Basic/SourceManager.h>
#include <set>
#include <string>
#include <vector>

namespace clang {
class Decl;
class Stmt;
}


class Utils {

public:

   static std::string extractCommercialCodePath(const std::string &fileNamePath);
   static std::string extractPackagePath(const std::string &fileNamePath);
   static std::string removeFileExtension(const std::string &fileName);
   static std::string changeFilePathToInclude(const std::string &fileName);
   static std::string changeFileExtension(const std::string &fileName,
                                          const std::string &newExtension);
   static std::string removeDashes(const std::string &fileNamePath);

   static std::string getDeclSourceFile(const clang::Decl *decl, const clang::SourceManager &srcMgr);

   // append the row and column to the file name name string
   static std::string getDeclSourceFileLine(const clang::Decl *decl,
                                            const clang::SourceManager &srcMgr);
   static std::string getStmtSourceFile(const clang::Stmt           *stmt,
                                        const clang::SourceManager  &srcMgr);
   // append the row and column to the file name name string
   static std::string getStmtSourceFileLine(const clang::Stmt          *stmt,
                                            const clang::SourceManager &srcMgr);
   //old implem to be rmd
   static void getIncludePaths(const std::string &          package,
                               std::vector<std::string> &   include_folders);
   static void getSourcePaths(const std::string &            package,
                              std::vector<std::string> &     source_folders);
   
   //new implem
   static void getIncludePaths(const std::set<boost::filesystem::path> &  packages,
                               std::set<boost::filesystem::path> &        include_dir);
   static void getSourcesPaths(const std::set<boost::filesystem::path> &   packages,
                               std::set<boost::filesystem::path> &         sources_dir);

   
};


#endif // _UTBuilder_Utils_h__

