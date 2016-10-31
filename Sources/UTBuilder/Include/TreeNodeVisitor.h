#ifndef _UTBuilder_TreeNodeVisitor_h__
#define _UTBuilder_TreeNodeVisitor_h__


#include "TreeNode.h"

template<typename Derived, typename T>
class BaseNode;

class Node;
class QualTypeNode;
class FunctionDeclNode;


class Visitor
{
public:
  
    void visit(Node* node);

private:
    
    void dispatch(Node* node)
    {
    }

    void dispatch(QualTypeNode* node)
    {
    }
    
    void dispatch(FunctionDeclNode* node)
    {
    }
};


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


#endif // _UTBuilder_TreeNodeVisitor_h__

