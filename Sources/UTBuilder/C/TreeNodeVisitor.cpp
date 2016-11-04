#include "TreeNodeVisitor.h"

#include "TreeNode.h"
#include "TreeNodeActions.h"

/*
void Visitor::process(BaseNode* node)
{
  
   if ( _action->run(node) )
   {
      const auto& children = node->getChildren();

      for (const auto& child : children ) {
	  child.second.get()->visit(*this);
      }
   }
   
}

*/