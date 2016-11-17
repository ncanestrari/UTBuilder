
#include "StructVisitor.h"

#include <boost/filesystem.hpp>

#include "ASTinfo.h"


StructVisitor::StructVisitor(clang::ASTContext*              context,
      const std::vector<std::string>& fileNames,
      ClangCompiler& compiler 
      )
: Visitor(context, fileNames, compiler)
{
}


bool StructVisitor::VisitDecl(clang::Decl *decl)
{
   const clang::RecordDecl *structure = llvm::dyn_cast<clang::RecordDecl>(decl);

   if (structure == nullptr) {
      return true;
   }

   // RecordDecl are class struct and union, we trigger only for struct
   if (structure->isStruct()) {

      const clang::RecordDecl *structdef = structure->getDefinition();
      // Forward/Partial definition skipped (should not happen)
      if (structdef == nullptr) {
         return true;
      }

      const clang::Type *declType = structdef->getTypeForDecl();


      if (_info->getFunctionDeclTypes().find(declType) ==  _info->getFunctionDeclTypes().end()) {
         return true;
      }
      //  check if this RecordDecl is already in a TypedefDecl
      for (auto typedefIter : _info->getTypedefNameDecls() ) {
         //canonical Types: http://clang.llvm.org/docs/InternalsManual.html#canonical-types
         const clang::QualType typedefQualType = typedefIter->getCanonicalDecl()->getUnderlyingType();
         const clang::Type *typedefDeclType = typedefQualType->getCanonicalTypeInternal().getTypePtrOrNull();

         if (declType != typedefDeclType) {
            _info->addStructureDecl(structure);
         }
      }

   }

   return true;
}

