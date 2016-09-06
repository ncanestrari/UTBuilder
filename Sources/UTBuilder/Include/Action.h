#ifndef __ACTION_H__
#define __ACTION_H__

#include <clang/Frontend/FrontendAction.h>

class Writer;


class Action : public clang::ASTFrontendAction
{
  
public:

   Action() {};

   virtual clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance&  compiler,
                                                 llvm::StringRef           inFile) override;

   virtual void EndSourceFileAction() override;
   
   
   
private:
  
    std::shared_ptr<Writer> _writer;
    
};


#endif /* !__ACTION_H__ */
