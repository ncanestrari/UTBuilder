
#ifndef _UTBuilder_NameValueTypeVisitor_h__
#define _UTBuilder_NameValueTypeVisitor_h__

#include <clang/AST/Decl.h>

#include <json/json.h>


class NameValueNode;
class NodeAction;

class NodeVisitor
{
public:
  
   NodeVisitor(NodeAction* action ) 
   : _action( action)
   {}
   
   void process(NameValueNode*  node);
    
private:
    
    NodeAction* _action;
};

class TwoNodesVisitor
{
public:
  
   TwoNodesVisitor(NodeAction* action ) 
   : _action( action)
   {}
 
   void process(NameValueNode*  nodeA, NameValueNode*  nodeB);
    
private:
    
    NodeAction* _action;
};



class OneNodeVisitor
{
public:
    
   OneNodeVisitor() {}
   virtual ~OneNodeVisitor() {}
   
   virtual bool run(NameValueNode*  node) = 0;  
};


class WriteJsonVisitor : public OneNodeVisitor
{
   Json::Value& _jsonRoot;
public:
    
   WriteJsonVisitor(Json::Value& jsonRoot);
   
   virtual bool run(NameValueNode*  node) override final;
  
};


#endif // _UTBuilder_NameValueTypeVisitor_h__

