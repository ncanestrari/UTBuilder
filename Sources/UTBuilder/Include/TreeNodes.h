
#ifndef _UTBuilder_TreeNodes_h__
#define _UTBuilder_TreeNodes_h__

#include <clang/AST/Decl.h>
#include <map>
#include <string>
#include <iostream>
#include <exception>

#define DECIMAL_BASE (10)


// class NodeVisitor;
// class NodeAction;
// class OneNodeVisitor;


class NameValueNode {

   /** 
    * if this node is an object _name is empty
    * if this node is an array element _name is the index in the array
    */
   const std::string _name;
   
   
   const std::string _value;   
   
protected:
   
   std::map< std::string, std::unique_ptr<NameValueNode> > _children;
   
   
   NameValueNode() = delete;
   
   NameValueNode(const NameValueNode& other);
   
   explicit NameValueNode(const char *name, const char *value);
   
   
   bool _isNameInteger(void) const;
   
public:

  
   static const char* _arrayIndex[];
   
   static const unsigned int _arrayIndexSize; 

   static const char* _arrayElementObject;
   
   
   virtual ~NameValueNode() {}
   
   
   static NameValueNode* createValue(const char *name, const char *value = "");
   
   static NameValueNode* createObject(const char *name);
   
   static NameValueNode* createArray(const char *name);
   
   static NameValueNode* createArrayElement(unsigned int index, const char* val = "");
   

   virtual NameValueNode* clone(const char * value = "") const;
   
   
   
//    virtual void visit( NodeVisitor* visitor );
//    virtual bool execute( NodeAction* action );
//    virtual bool execute( OneNodeVisitor* action );
   
   
   const std::string &getName(void) const { return _name; }
   const std::string &getValue(void) const { return _value; }
   

   virtual const void* getType() const { return nullptr; }
   
   
   bool isArray() const { return (_value == "array" ); }
   bool isObject() const { return (_value == "object" );}
   
   const bool isArrayElement(void) const { return (_value == NameValueNode::_arrayElementObject || _isNameInteger() ); }
   const bool isArrayElementObject(void) const { return (_value == NameValueNode::_arrayElementObject ); }
   const bool isArrayElementValue(void) const { return ( _isNameInteger() ); }
   
   
   int getIndex(void) const;
   
   unsigned int getNumChildern(void) const { return _children.size(); }
   const std::map< std::string, std::unique_ptr<NameValueNode> >& getChildren(void) const { return _children; }
   
   void addChild(NameValueNode* child);
   const NameValueNode* getChild(const char *name) const;

};



class QualTypeNode : public NameValueNode 
{
   
   // switch to mutable to avoid  a const_cast<>() in function void* getType() const and many other const in static_cast<> 
//    mutable T _type;
   clang::QualType _type;
   
protected:
   
   explicit QualTypeNode(const char *name, const clang::QualType& type, const char *value = "\0");
   
   QualTypeNode() = delete;
   
   QualTypeNode(const QualTypeNode&);
   
   
public:
   
   virtual ~QualTypeNode() {}
  
   static QualTypeNode* create(const char *name, const clang::QualType& type = QualTypeNode::_defaultType, const char *value = "\0");

   
   virtual QualTypeNode* clone(const char * value = "") const override final;
   
   virtual const void* getType() const override final { return &_type; }
   
//    virtual void visit( NodeVisitor* visitor ) override final;
//    virtual bool execute( NodeAction* action ) override final;
//    virtual bool execute( OneNodeVisitor* action ) override final;
   
   static const clang::QualType _defaultType;
   
};


class FunctionDeclNode : public NameValueNode
{
  
   const clang::FunctionDecl* _type;
   
protected:
  
   FunctionDeclNode() = delete;
   
   FunctionDeclNode(const FunctionDeclNode& other);
   
   explicit FunctionDeclNode(const char *name, const clang::FunctionDecl* type, const char *value = "\0");
   
   
public:
  
   static const clang::FunctionDecl* const _defaultType;
  
   virtual ~FunctionDeclNode() {}
   
   virtual const void* getType() const override final{ return _type; }
   
   
   static FunctionDeclNode* create(const char *name, const clang::FunctionDecl* type = FunctionDeclNode::_defaultType, const char *value = "\0");
   
   virtual FunctionDeclNode* clone(const char * value = "") const override final;
 
   
//    virtual void visit( NodeVisitor* visitor ) override final;
//    virtual bool execute( NodeAction* action ) override final;
//    virtual bool execute( OneNodeVisitor* action ) override final;
   
};


#endif // _UTBuilder_TreeNodes_h__

