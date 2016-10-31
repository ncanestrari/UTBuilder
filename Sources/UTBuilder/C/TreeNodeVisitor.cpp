#include "TreeNodeVisitor.h"



void Visitor::visit(Node* node)
{
   dispatch(node);

   const auto& children = node->getChildren();

   for (const auto& child : node) {
      child->dispatch();
   }
}
