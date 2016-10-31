
#include "utils.h"
#include "globber.h"
#include "Results.h"

#include <boost/filesystem/convenience.hpp>
#include <clang/AST/Decl.h>
#include <clang/AST/Stmt.h>

using std::set;
using std::string;
using std::toupper;
using std::vector;


string utils::extractCommercialCodePath(const string &fileNamePath)
{
   size_t lastdot = fileNamePath.find("/CommercialCode/");
   if ( lastdot != string::npos ) {
      return fileNamePath.substr(0, lastdot) + "/CommercialCode/";
   }
   
   // try lower case letters
   lastdot = fileNamePath.find("/commercialcode/");
   if ( lastdot != string::npos ) {
      return fileNamePath.substr(0, lastdot) + "/commercialcode/";
   }
   
   return boost::filesystem::current_path().string();
}


string utils::extractPackagePath(const string &fileNamePath)
{
   string cc("/CommercialCode/");
   size_t lastdot = fileNamePath.find(cc);
   
   if (  lastdot != string::npos ) {
      string rest = fileNamePath.substr(lastdot + cc.length(), fileNamePath.length() - 1);
      lastdot = rest.find("/");
      return rest.substr(0, lastdot);
   }
   
   cc = "/commercialcode/";
   lastdot = fileNamePath.find(cc);
   
   if (  lastdot != string::npos ) {
      string rest = fileNamePath.substr(lastdot + cc.length(), fileNamePath.length() - 1);
      lastdot = rest.find("/");
      return rest.substr(0, lastdot);
   }
   
   return boost::filesystem::current_path().string();
}


void utils::getIncludePaths(const string   &package,
                            vector<string> &include_folders)
{
   string repo(package);
   //now str is the Repository
   const string matchers_[] = { "/Sources/*/Include/", "/Input_API/*/API/", "/Input_API/*/API/*/" };
   const vector<string> matchers(matchers_, matchers_ + sizeof(matchers_) / sizeof(string));
   for (const auto matcher : matchers) {
      string path = repo + matcher;
      vector<string> ret = glob(path);
      include_folders.insert(include_folders.end(), ret.begin(), ret.end());
   }
}

void utils::getSourcePaths(const string& package, vector< string >& source_folders)
{
   string repo(package);
   //now str is the Repository
   const string matchers = "/Sources/*/C/";

   string path = repo + matchers;
   vector<string> ret = glob(path);
   source_folders.insert(source_folders.end(), ret.begin(), ret.end());
}



string utils::removeFileExtension(const string &fileNamePath)
{
   size_t lastdot = fileNamePath.find_last_of(".");
   return (lastdot == string::npos) ? fileNamePath : fileNamePath.substr(0, lastdot);
}


string utils::changeFileExtension(const string &fileName, const string &newExt)
{
   return boost::filesystem::change_extension(fileName, newExt).string();
}


string utils::changeFilePathToInclude(const string &fileName)
{
   string filenameInclude = fileName;
   size_t pos = 0;
   pos = filenameInclude.find("/C/", pos);
   if ( pos != std::string::npos) {
     filenameInclude.replace(pos, 3, "/Include/");
   }
   return filenameInclude;
}


string utils::removeDashes(const string &fileNamePath)
{
   string fileName = boost::filesystem::path(fileNamePath).filename().string();

   if (fileName.empty()) {
      return fileName;
   }

   size_t pos = 0;
   fileName[pos] = toupper(fileName[pos]);

   pos = fileName.find("-", pos);
   while (pos != string::npos) {
      fileName.erase(pos, 1);
      fileName[pos] = toupper(fileName[pos]);
      pos = fileName.find("-", pos);
   }

   return boost::filesystem::path(fileName).parent_path().string() + fileName;
}


void utils::fillFunctionQualTypes(void)
{
   results::get().functionDeclTypes.clear();

   //canonical Types: http://clang.llvm.org/docs/InternalsManual.html#canonical-types
   for (auto funcToMock : FunctionsToMock::get().declKeySetMap) {
      const clang::FunctionDecl *funcDecl = funcToMock.first;
      const clang::QualType returnType = funcDecl->getReturnType();
      results::get().functionDeclTypes.insert(returnType->getCanonicalTypeInternal().getTypePtrOrNull());

      const int numParms = funcDecl->getNumParams();
      for (int i = 0; i < numParms; ++i) {
         const clang::ParmVarDecl *_currentParam = funcDecl->getParamDecl(i);
         const clang::QualType qualType = _currentParam->getOriginalType();

         results::get().functionDeclTypes.insert(qualType->getCanonicalTypeInternal().getTypePtrOrNull());
      }
   }

   for (auto functionToTest : FunctionsToUnitTest::get().declKeySetMap) {
      const clang::FunctionDecl *funcDecl = functionToTest.first;
      const clang::QualType returnType = funcDecl->getReturnType();
      results::get().functionDeclTypes.insert(returnType->getCanonicalTypeInternal().getTypePtrOrNull());
      const int numParms = funcDecl->getNumParams();

      for (int i = 0; i < numParms; ++i) {
         const clang::ParmVarDecl *_currentParam = funcDecl->getParamDecl(i);
         const clang::QualType qualType = _currentParam->getOriginalType();

         results::get().functionDeclTypes.insert(qualType->getCanonicalTypeInternal().getTypePtrOrNull());
      }
   }
}


std::string utils::getDeclSourceFile(const clang::Decl *decl, const clang::SourceManager &srcMgr)
{
   // source location
   const clang::SourceLocation srcLoc = decl->getSourceRange().getBegin();
   return srcMgr.getFilename(srcLoc).str();
}


std::string utils::getDeclSourceFileLine(const clang::Decl *decl, const clang::SourceManager &srcMgr)
{
   // source location
   const clang::SourceLocation srcLoc = decl->getSourceRange().getBegin();
   return srcLoc.printToString(srcMgr);
}


std::string utils::getStmtSourceFile(const clang::Stmt *stmt, const clang::SourceManager &srcMgr)
{
   // source location
   const clang::SourceLocation srcLoc = stmt->getSourceRange().getBegin();
   return srcMgr.getFilename(srcLoc).str();
}


std::string utils::getStmtSourceFileLine(const clang::Stmt *stmt, const clang::SourceManager &srcMgr)
{
   // source location
   const clang::SourceLocation srcLoc = stmt->getSourceRange().getBegin();
   return srcLoc.printToString(srcMgr);
}

