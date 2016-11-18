
#ifndef _UTBuilder_AllTypesfVisitor_h__
#define _UTBuilder_AllTypesfVisitor_h__

#include "Visitor.h"

class AllTypesVisitor : public Visitor<AllTypesVisitor> {

public:

   explicit AllTypesVisitor(clang::ASTContext*               context,
                            const std::vector<std::string>&  fileNames,
                            ClangCompiler&                   compiler);

   bool VisitDecl(clang::Decl *decl);

private:
   
   
   
};


#endif // _UTBuilder_AllTypesfVisitor_h__
