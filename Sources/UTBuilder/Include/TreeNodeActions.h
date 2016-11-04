#ifndef _UTBuilder_TreeNodeActions_h__
#define _UTBuilder_TreeNodeActions_h__


#include "TreeNode.h"


class BaseNode;

class Node;
class QualTypeNode;
class FunctionDeclNode;


class Action
{
public:
  
    bool run(BaseNode* node);

private:
  
    virtual bool visitNode(Node* node) {};
    virtual bool visitQualTypeNode(QualTypeNode* node) {};
    virtual bool visitFunctionDeclNode(FunctionDeclNode* node) {};
};

// class BuildTreeAction : public Action
// {
// public:
//   
//     virtual bool execute(Node* node) override final;
//     virtual bool execute(QualTypeNode* node) override final;
//     virtual bool execute(FunctionDeclNode* node) override final;
// 
// private:
//     
// };


class PrintAction : public Action
{
public:
  
    virtual bool visitNode(Node* node) override final;
    virtual bool visitQualTypeNode(QualTypeNode* node) override final;
    virtual bool visitFunctionDeclNode(FunctionDeclNode* node) override final;

private:
    
};


#endif // _UTBuilder_TreeNodeActions_h__

