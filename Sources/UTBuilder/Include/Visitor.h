#ifndef UTBUILDER_VISITOR_H_
#define UTBUILDER_VISITOR_H_ 

#include <clang/AST/RecursiveASTVisitor.h>

#include <string>

template <typename Derived> 
class Visitor : public clang::RecursiveASTVisitor<Derived>
{
protected:
   clang::ASTContext *_context;
   std::string _fileName;

public:

   explicit Visitor(clang::ASTContext*   context,
                    std::string          fileName) :  _context(context), _fileName(fileName) {};
   
};


#endif // UTBUILDER_VISITOR_H_