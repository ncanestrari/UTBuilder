
#ifndef UTBUILDER_VISITOR_H_
#define UTBUILDER_VISITOR_H_

#include <clang/AST/RecursiveASTVisitor.h>

#include <string>
#include <vector>

template <typename Derived>
class Visitor : public clang::RecursiveASTVisitor<Derived> {

public:
   explicit Visitor(clang::ASTContext*              context,
                    const std::vector<std::string>& fileNames) :  _context(context), _fileNames(fileNames) {};

protected:
   clang::ASTContext*              _context;
   const std::vector<std::string>& _fileNames;
};

#endif // UTBUILDER_VISITOR_H_

