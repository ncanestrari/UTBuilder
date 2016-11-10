#include "TreeNodesVisitor.h"

#include "TreeNodes.h"
#include "TreeNodesActions.h"


void NodeVisitor::process(NameValueNode*  node)
{
  
   if ( node->execute(_action) )
   {
      const auto& children = node->getChildren();

      for (const auto& child : children ) {
	  child.second.get()->visit(this);
      }
   }
   
}

void TwoNodesVisitor::process(NameValueNode*  nodeA, NameValueNode*  nodeB)
{
   if ( nodeA->execute(_action) &&  nodeB->execute(_action) )
   {
      const auto& childrenA = nodeA->getChildren();
//       const auto& childrenB = nodeB->getChildren();

      for (const auto& childA : childrenA ) {
	 
	 const char* name = childA.first.c_str();
	 const auto& childB = nodeB->getChild(name);
 	 
	 if ( childB ) {
// 	    childA.second.get()->visit(this);
// 	    childB.second.get()->visit(this);	    
	 }
      }
   }
}

WriteJsonVisitor::WriteJsonVisitor(Json::Value& jsonRoot)
: _jsonRoot(jsonRoot)
{
}
   
bool WriteJsonVisitor::run(NameValueNode*  node)
{
   
}

   