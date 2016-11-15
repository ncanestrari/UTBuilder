
#ifndef _UTBuilder_ClangCompiler_h__
#define _UTBuilder_ClangCompiler_h__


#include <clang/Frontend/CompilerInstance.h>
#include <json/json.h>

#include "ProjectDescription.h"
#include "Results.h"


class ClangCompiler
{
public:
   
   ClangCompiler() = default;
   ~ClangCompiler() = default;
   
   void computeAST(const OptionParser& optionParser);

      
   //getters
   const clang::CompilerInstance & getInstance(void){ return _compiler; }
   const clang::CompilerInstance & getInstance(void) const { return _compiler; }

   const ProjectDescription & getProjectDescription(void){ return _projectDescription; }
   const ProjectDescription & getProjectDescription(void) const { return _projectDescription; }
   
   const OptionParser* getOptionParser() const { return _optionParser;}
   
   ASTinfo& getASTinfo()  { return _info; }
   const ASTinfo& getASTinfo()  const { return _info; }
   
   const clang::SourceManager& getSourceManager() const { return _compiler.getSourceManager(); }

private:

    
   clang::CompilerInstance _compiler;
   ProjectDescription _projectDescription;
   
   ASTinfo _info;
   
   const OptionParser* _optionParser;
};


#endif /* _UTBuilder_ClangAST_h__ */

