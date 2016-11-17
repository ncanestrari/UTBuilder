
#include "TypedefVisitor.h"

#include "ASTinfo.h"
#include "Utils.h"

#include <boost/filesystem.hpp>
#include <clang/AST/ASTContext.h>
#include <iostream>


TypedefVisitor::TypedefVisitor(clang::ASTContext*              context,
                               const std::vector<std::string>& fileNames,
                               ClangCompiler&                  compiler)
   : Visitor(context, fileNames, compiler)
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
   const std::string declSrcFile = Utils::getDeclSourceFile(type_def, _context->getSourceManager());

   // TODO: fix this
   // temporary check
   // WEAK CHECK if declaration is in CommercialCode path
   if (declSrcFile.find(boost::filesystem::current_path().string()) == std::string::npos) {
      return true;
   }

   if ( _info->getFunctionDeclTypes().find(declType) ==  _info->getFunctionDeclTypes().end()) {
      return true;
   }
   
   _info->addTypedefNameDecl(type_def);
   
   return true;
}

