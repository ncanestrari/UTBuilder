
#include "TreeNodesActions.h"

#include "TreeNodes.h"



bool PrintAction::run(NameValueNode* node)
{
//    const NodeType type = *static_cast<const NodeType*>(node->getType());
   std::cout << "name=" << node->getName() << "\tvalue=" << node->getName() << std::endl;
   return true;
}

bool PrintAction::run(QualTypeNode* node)
{
   const clang::QualType type = *static_cast<const clang::QualType*>(node->getType());
   std::cout << "name=" << node->getName() << "\tvalue=" << node->getName() << "\tQualType=" << type.getAsString() << std::endl;
   return true;
}

bool PrintAction::run(FunctionDeclNode* node)
{
   const clang::FunctionDecl* type = static_cast<const clang::FunctionDecl*>(node->getType());
   std::cout << "name=" << node->getName() << "\tvalue=" << node->getName() << "\tconst FunctionDecl*=" << type->getNameAsString() << std::endl;
   return true;
}


bool PrintJsonAction::run(NameValueNode* node)
{
//    const NodeType type = *static_cast<const NodeType*>(node->getType());
   std::cout << "name=" << node->getName() << "\tvalue=" << node->getName() << std::endl;
   return true;
}

bool PrintJsonAction::run(QualTypeNode* node)
{
   const clang::QualType type = *static_cast<const clang::QualType*>(node->getType());
   std::cout << "name=" << node->getName() << "\tvalue=" << node->getName() << "\tQualType=" << type.getAsString() << std::endl;
   return true;
}

bool PrintJsonAction::run(FunctionDeclNode* node)
{
   const clang::FunctionDecl* type = static_cast<const clang::FunctionDecl*>(node->getType());
   std::cout << "name=" << node->getName() << "\tvalue=" << node->getName() << "\tconst FunctionDecl*=" << type->getNameAsString() << std::endl;
   return true;
}

