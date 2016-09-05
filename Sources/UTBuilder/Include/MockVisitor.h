#ifndef _UTBuilder_MockVisitor_h__
#define _UTBuilder_MockVisitor_h__ 

#include "Visitor.h"


class MockVisitor : public Visitor<MockVisitor>
{
public:
   
   MockVisitor(clang::ASTContext*   context,
               std::string          fileName);
                  
   bool VisitCallExpr(clang::CallExpr *call);
   
};


#endif // _UTBuilder_MockVisitor_h__

