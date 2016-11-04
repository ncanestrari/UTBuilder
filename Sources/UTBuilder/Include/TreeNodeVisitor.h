#ifndef _UTBuilder_TreeNodeVisitor_h__
#define _UTBuilder_TreeNodeVisitor_h__


// #include "TreeNode.h"


class Visitor;


class BaseNode;

class Action;
class Node;
class QualTypeNode;
class FunctionDeclNode;


class Visitor
{
public:
  
   Visitor(Action* action ) 
   : _action( action)
   {}
   
   void process(BaseNode*  node);

    
private:
    
//     void dispatch(Node* node)
//     {
//     }
// 
//     void dispatch(QualTypeNode* node)
//     {
//     }
//     
//     void dispatch(FunctionDeclNode* node)
//     {
//     }
    
    
    Action* _action;
};


/*
template<typename Derived, typename T>
class BaseVisitor
{
public:
  
    void visit(const BaseNode<Derived,T>* node)
    {
      
    }

private:

    Derived& derived()
    {
        return *static_cast<Derived*>(this);
    }
    
};
*/

#endif // _UTBuilder_TreeNodeVisitor_h__

