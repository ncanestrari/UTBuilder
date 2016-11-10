
#ifndef _UTBuilder_TreeNodesActions_h__
#define _UTBuilder_TreeNodesActions_h__

#include "TreeNodes.h"
// #include <clang/AST/Decl.h>

#include <json/json.h>

// class NameValueNode;
// 
// template <typename T>
// class TypeNameValueNode;


class NodeAction
{
public:
  
   virtual bool run( NameValueNode* node) = 0;
   virtual bool run( QualTypeNode* node) = 0;
   virtual bool run( FunctionDeclNode* node) = 0;

   virtual ~NodeAction() {}
};


class PrintAction : public NodeAction
{
public:
  
    virtual bool run(NameValueNode* node) override final;
    virtual bool run(QualTypeNode* node)  override final;
    virtual bool run(FunctionDeclNode* node) override final;

private:
    
};

class PrintJsonAction : public NodeAction
{
public:
    PrintJsonAction(Json::Value& jsonRoot);
    
    virtual bool run(NameValueNode* node) override final;
    virtual bool run(QualTypeNode* node)  override final;
    virtual bool run(FunctionDeclNode* node) override final;

private:
    
   const NodeVisitor* _parent;
   Json::Value &jsonRoot;
   
};


#endif // _UTBuilder_TreeNodesActions_h__

