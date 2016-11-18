#ifndef _UTBuilder_ASTinfo_h__
#define _UTBuilder_ASTinfo_h__

#include <string>
#include <set>
#include <map>
#include <unordered_map>

#include <clang/AST/Decl.h>

// namespace clang {
// class FunctionDecl;
// class RecordDecl;
// class TypedefNameDecl;
// class Type;
// }


typedef std::set<const clang::FunctionDecl *> FunctionDeclSet;
typedef std::map<const clang::FunctionDecl *, FunctionDeclSet > FunctionDeclKeySetMap;
typedef std::map< std::string, const clang::FunctionDecl * > NameFunctionDeclMap;


// FunctionsToUnitTest and FunctionsToMock are the same class but with different meanings for its member vars

class FunctionsToUnitTest
{
public:

   void clear();

   /**
    * each key is a FunctionDecl* of functions ro unit test
    * each set stores the FunctionDecl* of the mock functions called inside the function to unit test
    *
    * this map is filled in two steps
    *
    * 1st step: in FuncUTDefVisitor::VisitDecl
    * the map is filled with keys of FunctionDecl* to unit test and with empty set value
    *
    * 2nd step: the mockVisitor::VisitCallExpr function
    * for each key of FunctionDecl* to unit test the set is filled with the FunctionDecl* of functions to mock
    */
   FunctionDeclKeySetMap _declKeySetMap; //functionsToUnitTestMap;

   // only for C: no overloaded and overriden functions
   NameFunctionDeclMap   _nameDeclMap; //functionsToUnitTestName;

};



class FunctionsToMock
{
public:

   void clear();

   /**
    * each key is a FunctionDecl* of mock functions
    * each set stores the FunctionDecl* of the caller ( all the functions to unit test that call the mock function of the key
    * this map is filled in the mockVisitor::VisitCallExpr function
    */
   
   // functionsToMockMap;
   FunctionDeclKeySetMap  _declKeySetMap;

   // only for C: no overloaded and overriden functions
   NameFunctionDeclMap    _nameDeclMap;

};



class ASTinfo
{
public:
   
   static const std::set<const clang::FunctionDecl *> _emptySet;
   
   ASTinfo() = default;
   ~ASTinfo() = default;
   
   void clear();

   void addStructureDecl(const clang::RecordDecl * decl) { _structureDecls.insert(decl); }
   void addTypedefNameDecl(const clang::TypedefNameDecl * decl) { _typedefNameDecls.insert(decl); }
   void addIncludeFile(const std::string& incl) { _includeFilesForUnitTest.insert(incl); }
   void addFunctionDeclType(const clang::Type * funcDeclType) { _functionDeclTypes.insert(funcDeclType); }
   
   
   void addFunctionToUnitTest(const clang::FunctionDecl* funcDecl, const clang::FunctionDecl* mockFuncDecl = nullptr );
   
   void addFunctionToMock(const clang::FunctionDecl* mockFuncDecl, const clang::FunctionDecl* callerFuncDecl);

   void addAllTypes(const clang::QualType& qualType) { _allTypesMap[qualType.getAsString()] = qualType; }
   
//    IMPORTANT: call this function after the _functionsToUnitTest and _functionsToMock are set
//    this is called in Consumer::HandleTranslationUnit()
   void fillFunctionQualTypes(void);
   
   
   const std::set<const clang::RecordDecl *>& getStructureDecls() const { return _structureDecls; }
   const std::set<const clang::TypedefNameDecl *>& getTypedefNameDecls() const { return _typedefNameDecls; }
   const std::set<std::string>& getIncludeFiles() const { return _includeFilesForUnitTest; }
   const std::set<const clang::Type *>& getFunctionDeclTypes() const { return _functionDeclTypes; }
   
   
   const std::set<const clang::FunctionDecl *>& getMockCalleesForFunction(const clang::FunctionDecl * funcDecl);
   const std::set<const clang::FunctionDecl *>& getFunctionCallersForMock(const clang::FunctionDecl * funcDecl);
   
   
   const FunctionDeclKeySetMap& getFunctionsToUnitTestMap() const { return _functionsToUnitTest._declKeySetMap; }
   const NameFunctionDeclMap& getFunctionsToUnitTest() const { return _functionsToUnitTest._nameDeclMap; }
   
   const FunctionDeclKeySetMap& getFunctionsToMockMap() const { return _functionsToMock._declKeySetMap; }
   const NameFunctionDeclMap& getFunctionsMock() const { return _functionsToMock._nameDeclMap; }
   const std::map<std::string, clang::QualType>& getAllTypesMap(void) const { return _allTypesMap; }
private:
   
   FunctionsToUnitTest  _functionsToUnitTest;
   FunctionsToMock	_functionsToMock;
   
   std::set<const clang::RecordDecl *>        _structureDecls;
   std::set<const clang::TypedefNameDecl *>   _typedefNameDecls;
   std::set<std::string>                      _includeFilesForUnitTest;
   std::set<const clang::Type *>              _functionDeclTypes;
   std::map<std::string, clang::QualType> _allTypesMap;
   
};

#endif // _UTBuilder_ASTinfo_h__
