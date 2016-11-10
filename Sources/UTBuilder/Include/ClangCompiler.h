
#ifndef _UTBuilder_ClangCompiler_h__
#define _UTBuilder_ClangCompiler_h__


#include <clang/Frontend/CompilerInstance.h>
#include <json/json.h>

#include "ProjectDescription.h"



class ClangCompiler
{
public:
   
   ClangCompiler() = default;
   ~ClangCompiler() = default;
   
   void computeAST(void);

      
   //getters
   const clang::CompilerInstance & getInstance(void){ return _compiler; }
   const clang::CompilerInstance & getInstance(void) const { return _compiler; }

   const ProjectDescription & getProjectDescription(void){ return _projectDescription; }
   const ProjectDescription & getProjectDescription(void) const { return _projectDescription; }
   
private:

    
   clang::CompilerInstance _compiler;
   ProjectDescription _projectDescription;
};


#endif /* _UTBuilder_ClangAST_h__ */

