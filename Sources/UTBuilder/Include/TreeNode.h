
#ifndef _UTBuilder_TreeNode_h__
#define _UTBuilder_TreeNode_h__

#include <clang/AST/Decl.h>
#include <map>
#include <string>
#include <iostream>
#include <exception>

#include "TreeNodeVisitor.h"

#define DECIMAL_BASE (10)


template<typename Derived, typename T>
class BaseVisitor;

// typename Derived is not used yet
template<typename Derived, typename T>
class BaseNode
{
   const std::string _name;
   const std::string _value;
   
   std::map< std::string, std::unique_ptr<BaseNode> > _children;
  
protected:
   const T& _type;
   
      
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
   
    Derived& derived()
    {
        return *static_cast<Derived*>(this);
    }
   
public:
  
   T& getType() const
   {
//       return static_cast<Derived*>(this)->getNodeType();
      return _type;
   }

   const T& getDefaultType() const
   {
//       return static_cast<Derived*>(this)->getNodeType();
      derived()._defaultType;
   }
   
   void accept(BaseVisitor<Derived,T>& visitor) 
   {
        visitor.visit( derived() );
   }
    
   static const char* _arrayIndex[];
   static const unsigned int _arrayIndexSize; 

   static const char* _arrayElementObject;
   
   static BaseNode* create(const char *name, const char *value = "") {
      return new BaseNode(name, value);
   }
   
   static BaseNode* createObject(const char *name) {
      return new BaseNode(name, "object");
   }
   
   static BaseNode* createArray(const char *name) {
      return new BaseNode(name, "array");
   }
   
   static BaseNode* createArrayElement(unsigned int index, const char* val = "") {

      if ( val == "") {
         val = BaseNode::_arrayElementObject;
      }
      return new BaseNode( std::to_string(index).c_str(), val );
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
   const BaseNode* getChild(const char *name) const
   {
      auto iter = _children.find(name);
      if (iter == _children.end()) {
         std::cout << "ERROR: node " << _name << " has no child named '" << name << "'" << std::endl; 
         return nullptr;
      }
      return iter->second.get();
   }


   /*
   virtual const BaseNode* addChild(const char *name, const char *value )
   {
      _children[name] = std::unique_ptr<BaseNode>(new BaseNode(name, T(), value) );
      return getChild(name);
   }
   */
};



class Node : public BaseNode<Node, int>
{
   
protected:
  
   explicit Node(const char *name, const char *value = "\0")
      : BaseNode<Node, int>(name, Node::_default, value)
   {}
   
public:
  
   static const int _default;
   
   static Node* create(const char *name, const char *value = "\0") 
   {
      return new Node(name, value);
   }
   
   int getNodeType() const
   {
      return 0;
   }
   
   
};


class QualTypeNode : public BaseNode<QualTypeNode, clang::QualType>
{
protected:
  
   explicit QualTypeNode(const char *name, clang::QualType type = clang::QualType(), const char *value = "\0")
      : BaseNode<QualTypeNode, clang::QualType>(name, type, value)
   {}
   
public:
  
   static const clang::QualType _default;
  
   static QualTypeNode* create(const char *name, clang::QualType qualType = QualTypeNode::_default, const char *value = "\0");
   
   clang::QualType getNodeType() const
   {
      return _type;
   }
};

class FunctionDeclNode : public BaseNode<FunctionDeclNode, const clang::FunctionDecl*>
{
  
protected:
  
   explicit FunctionDeclNode(const char *name, const clang::FunctionDecl* type, const char *value = "\0")
      : BaseNode<FunctionDeclNode, const clang::FunctionDecl*>(name, type, value )
   {}
   
public:
  
   static clang::FunctionDecl* const _default;
  
   static FunctionDeclNode* create(const char *name, const clang::FunctionDecl* type = FunctionDeclNode::_default, const char *value = "\0")
   {
     return new FunctionDeclNode(name, type, value);
   }
   
   const clang::FunctionDecl* getNodeType() const
   {
      return _type;
   }
};





#endif // _UTBuilder_TreeNode_h__

