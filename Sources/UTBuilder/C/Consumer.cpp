
#include "Consumer.h"

#include "MockVisitor.h"
#include "FuncUTDefVisitor.h"
#include "StructVisitor.h"
#include "TypedefVisitor.h"
#include "utils.h"

#include <clang/AST/ASTContext.h>


Consumer::Consumer(clang::ASTContext  *context,
                   std::string         fileName)
   : ASTConsumer()
   , _mockVisitor(nullptr)
   , _defVisitor(nullptr)
   , _declVisitor(nullptr)
   , _typedefVisitor(nullptr)
   , _structVisitor(nullptr)
{
   _mockVisitor = std::make_shared<MockVisitor>(context, fileName);
   _defVisitor = std::make_shared<FuncUTDefVisitor>(context, fileName);
   _declVisitor = std::make_shared<FuncUTDeclVisitor>(context, fileName);
   _typedefVisitor = std::make_shared<TypedefVisitor>(context, fileName);
   _structVisitor = std::make_shared<StructVisitor>(context, fileName);
}


void Consumer::HandleTranslationUnit(clang::ASTContext &ctx)
{
   // the order of the next three lines is IMPORTANT!
   // the filling of the KeyVectorMap structures depend on the right order of
   // these visitors traverse calls
   _defVisitor->TraverseDecl(ctx.getTranslationUnitDecl());
   _declVisitor->TraverseDecl(ctx.getTranslationUnitDecl());
   _mockVisitor->TraverseDecl(ctx.getTranslationUnitDecl());

   // fill the function params set
   utils::fillFunctionQualTypes();

   _typedefVisitor->TraverseDecl(ctx.getTranslationUnitDecl());
   _structVisitor->TraverseDecl(ctx.getTranslationUnitDecl());
}

