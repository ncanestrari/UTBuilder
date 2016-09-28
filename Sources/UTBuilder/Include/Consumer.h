
#ifndef __CONSUMER_H__
#define __CONSUMER_H__

#include <clang/AST/ASTConsumer.h>
#include <memory>
#include <string>


class FuncUTDefVisitor;
class FuncUTDeclVisitor;
class StructVisitor;
class TypedefVisitor;
class MockVisitor;

class Consumer : public clang::ASTConsumer {

public:
   Consumer(clang::ASTContext   *context,
            std::string          fileName);
   ~Consumer() {}

   virtual void HandleTranslationUnit(clang::ASTContext &ctx) override;

private:

   // could be these visitors just classes and not pinters ?
   std::shared_ptr<FuncUTDefVisitor>    _defVisitor;
   std::shared_ptr<FuncUTDeclVisitor>   _declVisitor;
   std::shared_ptr<StructVisitor>       _structVisitor;
   std::shared_ptr<MockVisitor>         _mockVisitor;
   std::shared_ptr<TypedefVisitor>	_typedefVisitor;

};


#endif /* !__CONSUMER_H__ */

