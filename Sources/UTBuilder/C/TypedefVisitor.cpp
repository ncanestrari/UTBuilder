
#include "TypedefVisitor.h"

#include "Results.h"
#include "utils.h"

#include <boost/filesystem.hpp>
#include <clang/AST/ASTContext.h>
#include <iostream>


TypedefVisitor::TypedefVisitor(clang::ASTContext *context, std::string fileName)
   : Visitor(context, fileName)
{}


bool TypedefVisitor::VisitDecl(clang::Decl *decl)
{

   const clang::TypedefNameDecl *type_def = llvm::dyn_cast<clang::TypedefNameDecl>(decl);
   if (type_def == nullptr) {
      return true;
   }

   //canonical Types: http://clang.llvm.org/docs/InternalsManual.html#canonical-types
   const clang::QualType qualType = type_def->getCanonicalDecl()->getUnderlyingType();
   const clang::Type *declType = qualType->getCanonicalTypeInternal().getTypePtrOrNull();

   if (declType == nullptr) {
      return true;
   }

   // get declaration source location
   const std::string declSrcFile = utils::getDeclSourceFile(type_def, _context->getSourceManager());

   // TODO: fix this
   // temporary check
   // WEAK CHECK if declaration is in CommercialCode path
   if (declSrcFile.find("CommercialCode") == std::string::npos) {
      return true;
   }


   if (results::get().functionDeclTypes.find(declType) ==  results::get().functionDeclTypes.end()) {
      return true;
   }

   results::get().typedefNameDecls.insert(type_def);

   return true;
}

