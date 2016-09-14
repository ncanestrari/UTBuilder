#ifndef _UTBuilder_Results_h__
#define _UTBuilder_Results_h__

#include <string>
#include <set>
#include <map>

namespace clang {
   class FunctionDecl;
   class RecordDecl;
   class TypedefNameDecl;
   class Type;
}


typedef std::set<const clang::FunctionDecl*> FunctionDeclSet;
typedef std::map<const clang::FunctionDecl*, FunctionDeclSet > FunctionDeclKeySetMap;
   
   
class results {
  
   results(void) {};
   ~results(void) {};
   
public:
  
   static results& get(void);
   
   
   void clear();
   
   /**
    * each key is a FunctionDecl* of mock functions
    * each set stores the FunctionDecl* of the caller ( all the functions to unit test that call the mock function of the key 
    * this map is filled in the mockVisitor::VisitCallExpr function
    */
   FunctionDeclKeySetMap      functionsToMockMap;
   
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
   FunctionDeclKeySetMap      functionsToUnitTestMap;
   
   /*
   std::set<const clang::FunctionDecl*>      functionsToMock;
   std::set<const clang::FunctionDecl*>      functionsToUnitTest;
   */
   
   std::set<const clang::RecordDecl*>        structDecls;
 
   std::set<const clang::TypedefNameDecl*>   typedefNameDecls;
   std::set<std::string>                     includesForUnitTest;
   
   std::set<const clang::Type*>              functionDeclTypes;
};



#endif // _UTBuilder_Results_h__

