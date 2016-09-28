
#ifndef _UTBuilder_MockVisitor_h__
#define _UTBuilder_MockVisitor_h__

#include "Visitor.h"

namespace clang {
class FunctionDecl;
}

class MockVisitor : public Visitor<MockVisitor> {
   // in this var is stored the last FuncDecl traversed.
   // this  FuncDecl is used to know the caller of the mock function
   const clang::FunctionDecl *_lastFuncDecl;

public:
   MockVisitor(clang::ASTContext   *context,
               std::string          fileName);

   bool VisitCallExpr(clang::CallExpr *call);

   // iterate the decls only to store the lastFuncDecl;
   bool VisitDecl(clang::Decl *decl);

};


#endif // _UTBuilder_MockVisitor_h__

