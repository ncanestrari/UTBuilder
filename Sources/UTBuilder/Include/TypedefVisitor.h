
#ifndef _UTBuilder_TypedefVisitor_h__
#define _UTBuilder_TypedefVisitor_h__


#include "Visitor.h"


class TypedefVisitor : public Visitor<TypedefVisitor> {

public:

   explicit TypedefVisitor(clang::ASTContext*               context,
                           const std::vector<std::string>&  fileNames,
			   ClangCompiler& compiler 
  			);
   
   virtual ~TypedefVisitor() {}
   
   bool VisitDecl(clang::Decl *decl);

};


#endif // _UTBuilder_TypedefVisitor_h__

