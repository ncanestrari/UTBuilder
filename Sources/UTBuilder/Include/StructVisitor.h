
#ifndef _UTBuilder_StructVisitor_h__
#define _UTBuilder_StructVisitor_h__

#include "Visitor.h"

class StructVisitor : public Visitor<StructVisitor> {

public:
   explicit StructVisitor(clang::ASTContext*               context,
                          const std::vector<std::string>&  fileNames,
			  ClangCompiler& compiler 
 			);

   bool VisitDecl(clang::Decl *decl);

};


#endif // _UTBuilder_StructVisitor_h__

