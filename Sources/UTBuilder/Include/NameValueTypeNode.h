
#ifndef _UTBuilder_NameValueTypeNode_h__
#define _UTBuilder_NameValueTypeNode_h__

#include <clang/AST/Decl.h>
#include <map>
#include <string>
#include <iostream>


template <typename T>
class NameValueTypeNode {
   
   const std::string _name;
   const T           _type;
   const std::string _value;

   std::map< std::string, std::shared_ptr<NameValueTypeNode<T> > > _children;

public:

   
   explicit NameValueTypeNode(const char *name, T type = T(), const char *value = "\0")
      : _name(name)
      , _type(type)
      , _value(value)
   {}

   ~NameValueTypeNode() {}

   const std::string &getName(void) const
   {
      return _name;
   }


   const std::string &getValue(void) const
   {
      return _value;
   }

   T getType(void) const
   {
      return _type;
   }

   unsigned int getNumChildern(void)
   {
      return _children.size();
   }

   std::shared_ptr<NameValueTypeNode<T> > getChild(const char *name) const
   {
      auto iter = _children.find(name);
      if (iter == _children.end()) {
         std::cout << "ERROR: node " << _name << " has no child named '" << name << "'" << std::endl; 
         return nullptr;
      }
      return iter->second;
   }

   const std::map< std::string, std::shared_ptr<NameValueTypeNode<T> > >  &getChildren(void) const
   {
      return _children;
   }

   void addChild(std::shared_ptr<NameValueTypeNode<T> > child)
   {
      _children[child->_name] = child;
   }

   std::shared_ptr<NameValueTypeNode<T> > addChild(const char *name, T type, const char *value )
   {
      auto child = std::make_shared<NameValueTypeNode<T> >(name, type, value);
      _children[name] = child;

      return child;
   }

   void setValue(const char *value )
   {
      _value = value;
   }

};


#endif // _UTBuilder_NameValueTypeNode_h__

