
#include "Consumer.h"

#include "MockVisitor.h"
#include "FuncUTDefVisitor.h"
#include "StructVisitor.h"
#include "TypedefVisitor.h"
#include "Utils.h"

#include <clang/AST/ASTContext.h>


Consumer::Consumer(clang::ASTContext*              context,
                   const std::vector<std::string>& fileNames,
		   ClangCompiler& compiler )
   : ASTConsumer()
   , _mockVisitor(nullptr)
   , _defVisitor(nullptr)
   , _declVisitor(nullptr)
   , _typedefVisitor(nullptr)
   , _structVisitor(nullptr)
   , _info( &compiler.getASTinfo() )
{
   _mockVisitor = std::unique_ptr<MockVisitor>( new MockVisitor(context, fileNames, compiler) );
   _defVisitor = std::unique_ptr<FuncUTDefVisitor>( new FuncUTDefVisitor(context, fileNames, compiler) );
   _declVisitor = std::unique_ptr<FuncUTDeclVisitor>(new FuncUTDeclVisitor(context, fileNames, compiler) );
   _typedefVisitor = std::unique_ptr<TypedefVisitor>( new TypedefVisitor(context, fileNames, compiler) );
   _structVisitor = std::unique_ptr<StructVisitor>( new StructVisitor(context, fileNames, compiler) );
}

Consumer::~Consumer() = default;

void Consumer::HandleTranslationUnit(clang::ASTContext &ctx)
{
   // the order of the next three lines is IMPORTANT!
   // the filling of the KeyVectorMap structures depend on the right order of
   // these visitors traverse calls
   _defVisitor->TraverseDecl(ctx.getTranslationUnitDecl());
   _declVisitor->TraverseDecl(ctx.getTranslationUnitDecl());
   _mockVisitor->TraverseDecl(ctx.getTranslationUnitDecl());

   // fill the function params set
   _info->fillFunctionQualTypes();

   _typedefVisitor->TraverseDecl(ctx.getTranslationUnitDecl());
   _structVisitor->TraverseDecl(ctx.getTranslationUnitDecl());
}

