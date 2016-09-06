#ifndef _UTBuilder_Results_h__
#define _UTBuilder_Results_h__

#include <string>
#include <set>

namespace clang {
   class FunctionDecl;
   class RecordDecl;
   class TypedefNameDecl;
   class Type;
}


class results {
  
   results(void) {};
   ~results(void) {};
   
public:
  
   static results& get(void);
   
   
   void clear();
   
   
   std::set<const clang::FunctionDecl*>      functionsToMock;
   std::set<const clang::FunctionDecl*>      functionsToUnitTest;
   std::set<const clang::RecordDecl*>        structDecls;
 
   std::set<const clang::TypedefNameDecl*>   typedefNameDecls;
   std::set<std::string>                     includesForUT;
   
   std::set<const clang::Type*>              functionDeclTypes;
};



#endif // _UTBuilder_Results_h__

