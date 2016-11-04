
#ifndef _UTBuilder_TreeNode_h__
#define _UTBuilder_TreeNode_h__

#include <clang/AST/Decl.h>
#include <map>
#include <string>
#include <iostream>
#include <exception>

#include "TreeNodeVisitor.h"

#define DECIMAL_BASE (10)



class BaseNode {

   const std::string _name;
   const std::string _value;
   std::map< std::string, std::unique_ptr<BaseNode> > _children;
   
   
protected:
   
      BaseNode(const char *name, const char *value)
      : _name(name)
      , _value(value)
   {}
   
public:

   ~BaseNode() {}

   void visit( Visitor& visitor) 
   {
        visitor.process( this );
   }
   
   const std::string &getName(void) const { return _name; }
   const std::string &getValue(void) const { return _value; }
   
   virtual const void* getType() const { return nullptr; }

   
   unsigned int getNumChildern(void) const { return _children.size(); }
   const std::map< std::string, std::unique_ptr<BaseNode> >& getChildren(void) const { return _children; }
   void addChild(BaseNode* child) { _children[child->_name] = std::unique_ptr<BaseNode>(child); }
   
   const BaseNode* getChild(const char *name) const
   {
      auto iter = _children.find(name);
      if (iter == _children.end()) {
         std::cout << "ERROR: node " << _name << " has no child named '" << name << "'" << std::endl; 
         return nullptr;
      }
      return iter->second.get();
   }

};



enum NodeType 
{
   _object_type,
   _array_type,
   _arrayElement_type,
   _value_type,
};


class Node : public BaseNode 
{
   NodeType _type;
  
protected:
  
   explicit Node(const char *name, const NodeType& type = Node::_defaultType, const char *value = "\0")
      : BaseNode(name, value)
      , _type(type)
   {}
   
public:
  
   static const NodeType _defaultType;   
   
   virtual const void* getType() const override final{ return &_type; }
   
   static Node* createValue(const char *name, const char *value = "\0") 
   {
      return new Node(name, _value_type, value);
   }
   
   static Node* createObject(const char *name) 
   {
      return new Node(name, _object_type, "object");
   }
   
   static Node* createArray(const char *name) 
   {
      return new Node(name, _array_type, "array");
   }
   
   static Node* createArrayElement(unsigned int index) 
   {
      return new Node( std::to_string(index).c_str(), _arrayElement_type );
   }
   
  
   static const char* _arrayIndex[];
   static const unsigned int _arrayIndexSize; 

   static const char* _arrayElementObject;
   
};

/*
class QualTypeNode : public BaseNode
{
  
   clang::QualType _type;
   
protected:
  
   explicit QualTypeNode(const char *name, clang::QualType type, const char *value = "\0")
      : BaseNode(name, value)
      , _type(type)
   {}
   
public:
  
   static const clang::QualType _defaultType;
  
   virtual const void* getType() const override final{ return &_type; }
   
   static QualTypeNode* create(const char *name, clang::QualType qualType = QualTypeNode::_defaultType, const char *value = "\0");
   
};



class FunctionDeclNode : public BaseNode
{
  
   const clang::FunctionDecl* _type;
   
protected:
  
   explicit FunctionDeclNode(const char *name, const clang::FunctionDecl* type, const char *value = "\0")
      : BaseNode(name, value )
      , _type(type)
   {}
   
public:
  
   static const clang::FunctionDecl* const _defaultType;
  
   virtual const void* getType() const override final{ return _type; }
   
   static FunctionDeclNode* create(const char *name, const clang::FunctionDecl* type = FunctionDeclNode::_defaultType, const char *value = "\0")
   {
     return new FunctionDeclNode(name, type, value);
   }
   
};
*/


/*
template<typename Derived, typename T>
class BaseVisitor;

// typename Derived is not used yet
template<typename Derived, typename T>
class BaseNode
{
   const std::string _name;
   const std::string _value;
   
   std::map< std::string, std::unique_ptr<BaseNode > > _children;
  
protected:
   const T _type;
   
   
   explicit BaseNode(const char *name, const T& type, const char *value = "\0" )
      : _name(name)
      , _type(type)
      , _value(value)      
   {}
   
      bool _isNameInteger(void) const {
      if(_name.empty()) { return false; }
      char* p;
      strtol(_name.c_str(), &p, DECIMAL_BASE);
      return (*p == '\0');
   }
   
    Derived& derivedRef()
    {
        return *static_cast<Derived*>(this);
    }
   
    Derived* derivedPtr()
    {
        return static_cast<Derived*>(this);
    }
    
    static Derived* derived(BaseNode<Derived,T>* node)
    {
        return static_cast<Derived*>(node);
    }
    
public:
  
   BaseNode* getSuperClass() { return this; }
   
   const T& getType() const
   {
//       return static_cast<Derived*>(this)->getNodeType();
      return _type;
   }

   const T& getDefaultType() const
   {
//       return static_cast<Derived*>(this)->getNodeType();
      derivedPtr()->_defaultType;
   }
   
//    void accept(BaseVisitor<Derived,T>& visitor) 
//    {
//         visitor.visit( derived() );
//    }
   
   static void visit( BaseNode<Derived,T>* node,Visitor& visitor) 
   {
        visitor.process( BaseNode<Derived,T>::derived(node) );
   }
   
   void visit( Visitor& visitor) 
   {
        visitor.process( derivedPtr() );
   }
   

   
   

   ~BaseNode() {}

   
   const std::string &getName(void) const { return _name; }
   const std::string &getValue(void) const { return _value; }
   
   
   bool isArray() const { return (_value == "array" ); }
   bool isObject() const { return (_value == "object" );}
   
   const bool isArrayElement(void) const { return (_value == BaseNode::_arrayElementObject || _isNameInteger() ); }
   const bool isArrayElementObject(void) const { return (_value == BaseNode::_arrayElementObject ); }
   const bool isArrayElementValue(void) const { return ( _isNameInteger() ); }
   
   
   
   int getIndex(void) const
   {
      if ( isArrayElement() ) {
         if(_isNameInteger()){
            return stoi(_name);
         } else {
            throw std::logic_error(std::string("Error getIndexAsString: name is not an index\n"));
         }
      }
      else {
         return -1;
      }
   }
   
   unsigned int getNumChildern(void) const { return _children.size(); }
   const std::map< std::string, std::unique_ptr<BaseNode> >& getChildren(void) const { return _children; }
   void addChild(BaseNode* child) { _children[child->_name] = std::unique_ptr<BaseNode>(child); }
   
   const Derived* getChild(const char *name) const
   {
      auto iter = _children.find(name);
      if (iter == _children.end()) {
         std::cout << "ERROR: node " << _name << " has no child named '" << name << "'" << std::endl; 
         return nullptr;
      }
      return iter->second->derivedPtr();
   }

};


enum NodeType 
{
   _object_type,
   _array_type,
   _arrayElement_type,
   _value_type,
};


class Node : public BaseNode<Node, NodeType>
{
   
protected:
  
   explicit Node(const char *name, const NodeType& type = Node::_defaultType, const char *value = "\0")
      : BaseNode<Node, NodeType>(name, type, value)
   {}
   
public:
  
   static const NodeType _defaultType;

   static const char* _arrayIndex[];
   static const unsigned int _arrayIndexSize; 

   static const char* _arrayElementObject;
   
   
   static Node* createValue(const char *name, const char *value = "\0") 
   {
      return new Node(name, _value_type, value);
   }
   
   static Node* createObject(const char *name) {
      return new Node(name, _object_type, "object");
   }
   
   static Node* createArray(const char *name) {
      return new Node(name, _array_type, "array");
   }
   
   static Node* createArrayElement(unsigned int index, const char* val = "") {

      if ( val == "") {
         val = Node::_arrayElementObject;
      }
      return new Node( std::to_string(index).c_str(), _arrayElement_type, val );
   }
   
 
//    int getNodeType() const
//    {
//       return Node::_default;
//    }
   
   
};


class QualTypeNode : public BaseNode<QualTypeNode, clang::QualType>
{
protected:
  
   explicit QualTypeNode(const char *name, clang::QualType type, const char *value = "\0")
      : BaseNode<QualTypeNode, clang::QualType>(name, type, value)
   {}
   
public:
  
   static const clang::QualType _defaultType;
  
   static QualTypeNode* create(const char *name, clang::QualType qualType = QualTypeNode::_defaultType, const char *value = "\0");
   
//    const clang::QualType getNodeType() const
//    {
//       return _type;
//    }
};

class FunctionDeclNode : public BaseNode<FunctionDeclNode, const clang::FunctionDecl*>
{
  
protected:
  
   explicit FunctionDeclNode(const char *name, const clang::FunctionDecl* type, const char *value = "\0")
      : BaseNode<FunctionDeclNode, const clang::FunctionDecl*>(name, type, value )
   {}
   
public:
  
   static clang::FunctionDecl* const _defaultType;
  
   static FunctionDeclNode* create(const char *name, const clang::FunctionDecl* type = FunctionDeclNode::_defaultType, const char *value = "\0")
   {
     return new FunctionDeclNode(name, type, value);
   }
   
//    const clang::FunctionDecl* getNodeType() const
//    {
//       return _type;
//    }
};


*/


#endif // _UTBuilder_TreeNode_h__

