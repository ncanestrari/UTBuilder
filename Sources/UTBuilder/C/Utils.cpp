
#include "Utils.h"
#include "globber.h"
#include "Results.h"

#include <boost/filesystem/convenience.hpp>
#include <clang/AST/Decl.h>
#include <clang/AST/Stmt.h>
#include <iostream>

using boost::filesystem::current_path;
using boost::filesystem::path;

using std::set;
using std::string;
using std::toupper;
using std::vector;
using std::cout;
using std::endl;

string Utils::extractCommercialCodePath(const string &fileNamePath)
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


string Utils::extractPackagePath(const string &fileNamePath)
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


void Utils::getIncludePaths(const set<path> &  packages,
                            set<path> &        include_dir)
{
   path cwd = current_path();
   const string matchers_[] = { "Sources/*/Include/", "Input_API/*/API/", "Input_API/*/API/*/" };
   const vector<string> matchers(matchers_, matchers_ + sizeof(matchers_) / sizeof(string));

   for (const path &package : packages){
      current_path(package);
      for (const string &matcher : matchers) {
         set<path> local_matches;
         glob(matcher, local_matches);
         for (const path & match : local_matches){
            include_dir.insert(package / match);
         }
      }
      current_path(cwd);
   }
}


void Utils::getSourcesPaths(const set<path> &  packages,
                            set<path> &        sources_dir)
{
   path cwd = current_path();
   const string matcher = "Sources/*/C/";
   
   for (const path &package : packages){
      current_path(package);
      set<path> local_matches;
      glob(matcher, local_matches);
      for (const path & match : local_matches){
         sources_dir.insert(package / match);
      }
      current_path(cwd);
   }
}


string Utils::removeFileExtension(const string &fileNamePath)
{
   size_t lastdot = fileNamePath.find_last_of(".");
   return (lastdot == string::npos) ? fileNamePath : fileNamePath.substr(0, lastdot);
}


string Utils::changeFileExtension(const string &fileName, const string &newExt)
{
   return boost::filesystem::change_extension(fileName, newExt).string();
}


string Utils::removeDashes(const string &fileNamePath)
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


void Utils::fillFunctionQualTypes(void)
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


std::string Utils::getDeclSourceFile(const clang::Decl *decl, const clang::SourceManager &srcMgr)
{
   // source location
   const clang::SourceLocation srcLoc = decl->getSourceRange().getBegin();
   return srcMgr.getFilename(srcLoc).str();
}


std::string Utils::getDeclSourceFileLine(const clang::Decl *decl, const clang::SourceManager &srcMgr)
{
   // source location
   const clang::SourceLocation srcLoc = decl->getSourceRange().getBegin();
   return srcLoc.printToString(srcMgr);
}


std::string Utils::getStmtSourceFile(const clang::Stmt *stmt, const clang::SourceManager &srcMgr)
{
   // source location
   const clang::SourceLocation srcLoc = stmt->getSourceRange().getBegin();
   return srcMgr.getFilename(srcLoc).str();
}


std::string Utils::getStmtSourceFileLine(const clang::Stmt *stmt, const clang::SourceManager &srcMgr)
{
   // source location
   const clang::SourceLocation srcLoc = stmt->getSourceRange().getBegin();
   return srcLoc.printToString(srcMgr);
}

