#include "TreeNodeActions.h"

#include "TreeNode.h"

/*
bool Action::run(BaseNode* baseNode)
{
   if ( Node* node = dynamic_cast<Node*>(baseNode) ) {
      return visitNode(node);
   }
   else if ( QualTypeNode* node = dynamic_cast<QualTypeNode*>(baseNode) ) {
      return visitQualTypeNode(node);
   }
   else if (FunctionDeclNode* node = dynamic_cast<FunctionDeclNode*>(baseNode) ) {
      return visitFunctionDeclNode(node); 
   }
   else {
      std::cout << "WARNING! Action::execute(BaseNode* node) node type not defined" << std::endl;
   }
   
   return true;   
}


bool PrintAction::visitNode(Node* node)
{
   const NodeType type = *static_cast<const NodeType*>(node->getType());
   std::cout << "name=" << node->getName() << "\tvalue=" << node->getName() << "\ttype=" << type << std::endl;
   return true;
}

bool PrintAction::visitQualTypeNode(QualTypeNode* node)
{
   const clang::QualType type = *static_cast<const clang::QualType*>(node->getType());
   std::cout << "name=" << node->getName() << "\tvalue=" << node->getName() << "\tQualType=" << type.getAsString() << std::endl;
   return true;
}

bool PrintAction::visitFunctionDeclNode(FunctionDeclNode* node)
{
   const clang::FunctionDecl* type = static_cast<const clang::FunctionDecl*>(node->getType());
   std::cout << "name=" << node->getName() << "\tvalue=" << node->getName() << "\tconst FunctionDecl*=" << type->getNameAsString() << std::endl;
   return true;
}
*/