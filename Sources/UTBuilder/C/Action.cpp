
#include "Action.h"

#include "Consumer.h"

#include "Results.h"

#include <clang/AST/ASTContext.h>
#include <clang/Frontend/CompilerInstance.h>


clang::ASTConsumer* Action::CreateASTConsumer(clang::CompilerInstance& compiler,
                                              llvm::StringRef inFile)
{
   results::get().clear();
  
   _writer = std::unique_ptr<Writer>( new Writer(inFile.str(), getCompilerInstance().getSourceManager()) );
  
   return new Consumer(  &compiler.getASTContext(), inFile.str() );
}


void Action::EndSourceFileAction(){
  
   _writer->createFiles();
}

