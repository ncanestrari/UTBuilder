
#ifndef UTBUILDER_VISITOR_H_
#define UTBUILDER_VISITOR_H_

#include <clang/AST/RecursiveASTVisitor.h>

#include <string>
#include <vector>

#include "ClangCompiler.h"
#include "ASTinfo.h"

template <typename Derived>
class Visitor : public clang::RecursiveASTVisitor<Derived> {

public:
   explicit Visitor(clang::ASTContext*              context,
                    const std::vector<std::string>& fileNames,
		    ClangCompiler& compiler  ) 
   :  _context(context)
   , _fileNames(fileNames)
   , _info( &compiler.getASTinfo() )
   {};

   virtual ~Visitor() = default;
   
protected:
   clang::ASTContext*              _context;
   const std::vector<std::string>& _fileNames;
   ASTinfo* _info;
};

#endif // UTBUILDER_VISITOR_H_

