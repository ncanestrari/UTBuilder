
#ifndef __CONSUMER_H__
#define __CONSUMER_H__

#include <clang/AST/ASTConsumer.h>
#include <memory>
#include <string>
#include <vector>

#include "ClangCompiler.h"
#include "ASTinfo.h"

class FuncUTDefVisitor;
class FuncUTDeclVisitor;
class StructVisitor;
class TypedefVisitor;
class MockVisitor;

class Consumer : public clang::ASTConsumer {

public:
   Consumer(clang::ASTContext*               context,
            const std::vector<std::string>&  fileNames,
	    ClangCompiler& compiler
  	 );
   
   ~Consumer();

   virtual void HandleTranslationUnit(clang::ASTContext &ctx) override;

private:

   // could be these visitors just classes and not pinters ?
   std::unique_ptr<FuncUTDefVisitor>    _defVisitor;
   std::unique_ptr<FuncUTDeclVisitor>   _declVisitor;
   std::unique_ptr<StructVisitor>       _structVisitor;
   std::unique_ptr<MockVisitor>         _mockVisitor;
   std::unique_ptr<TypedefVisitor>	_typedefVisitor;
   
   ASTinfo* _info;

};


#endif /* !__CONSUMER_H__ */

