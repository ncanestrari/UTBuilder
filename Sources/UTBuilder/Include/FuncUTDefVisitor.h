
#ifndef _UTBuilder_FuncUTDefVisitor_h__
#define _UTBuilder_FuncUTDefVisitor_h__

#include "Visitor.h"

class FuncUTDefVisitor : public Visitor<FuncUTDefVisitor> {

public:

   explicit FuncUTDefVisitor(clang::ASTContext*               context,
                             const std::vector<std::string>&  fileNames,
			     ClangCompiler& compiler   );

   bool VisitDecl(clang::Decl *decl);

private:
   
   const OptionParser* _optionParser;
   
};



class FuncUTDeclVisitor : public Visitor<FuncUTDeclVisitor> {

public:

   explicit FuncUTDeclVisitor(clang::ASTContext*               context,
                              const std::vector<std::string>&  fileNames,
			      ClangCompiler& compiler   );

   bool VisitDecl(clang::Decl *decl);

};


#endif // _UTBuilder_FuncUTDefVisitor_h__

