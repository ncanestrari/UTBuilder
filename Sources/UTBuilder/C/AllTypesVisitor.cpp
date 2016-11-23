
#include "AllTypesVisitor.h"

#include <boost/filesystem.hpp>
#include <clang/Basic/SourceManager.h>
#include <clang/AST/ASTContext.h>

#include "ASTinfo.h"
#include "Utils.h"

AllTypesVisitor::AllTypesVisitor(clang::ASTContext*              context, 
                                  const std::vector<std::string>& fileNames,
                                  ClangCompiler&                  compiler)
   : Visitor(context, fileNames, compiler)
{
}


bool AllTypesVisitor::VisitDecl(clang::Decl *decl)
{
   const clang::TypedefNameDecl *type_def = llvm::dyn_cast<clang::TypedefNameDecl>(decl);
   if (type_def == nullptr) {
      return true;
   }

   //canonical Types: http://clang.llvm.org/docs/InternalsManual.html#canonical-types
   const clang::QualType qualType = type_def->getUnderlyingType(); //getCanonicalDecl()->getUnderlyingType();
   const clang::Type *declType = qualType->getCanonicalTypeInternal().getTypePtrOrNull();

//    if (declType == nullptr) {
//       return true;
//    }   
   
   const std::string name = type_def->getNameAsString();
   std::cout << "type:\t" << qualType.getAsString() << std::endl;
   
   _info->addAllTypes( name, type_def );

   return true;
}
