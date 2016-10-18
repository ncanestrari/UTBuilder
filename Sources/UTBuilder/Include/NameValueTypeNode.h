
#ifndef _UTBuilder_NameValueTypeNode_h__
#define _UTBuilder_NameValueTypeNode_h__

#include <clang/AST/Decl.h>
#include <map>
#include <string>
#include <iostream>
#include <exception>

#define DECIMAL_BASE (10)



class NameValueNode {

   const std::string _name;
   const std::string _value;
   std::map< std::string, std::unique_ptr<NameValueNode> > _children;
   
   
protected:
   
      NameValueNode(const char *name, const char *value)
      : _name(name)
      , _value(value)
   {}
   
public:


   static NameValueNode* createValue(const char *name, const char *value = "\0") {
      return new NameValueNode(name, value);
   }
   
   static NameValueNode* createObject(const char *name) {
      return new NameValueNode(name, "object");
   }
   
   static NameValueNode* createArray(const char *index) {
      return new NameValueNode(index, "array");
   }
   
   ~NameValueNode() {}

   const std::string &getName(void) const { return _name; }
   const std::string &getValue(void) const { return _value; }
   

   virtual void* getType() { return nullptr; }
   
   bool isArray() { return (_name == "array" ); }
   bool isObject() { return (_name == "object" );}
   
   unsigned int getNumChildern(void) { return _children.size(); }
   const std::map< std::string, std::unique_ptr<NameValueNode> >  &getChildren(void) const { return _children; }
   void addChild(NameValueNode* child) { _children[child->_name] = std::unique_ptr<NameValueNode>(child); }
   const NameValueNode* getChild(const char *name) const
   {
      auto iter = _children.find(name);
      if (iter == _children.end()) {
         std::cout << "ERROR: node " << _name << " has no child named '" << name << "'" << std::endl; 
         return nullptr;
      }
      return iter->second.get();
   }


   virtual const NameValueNode* addChild(const char *name, const char *value )
   {
      _children[name] = std::unique_ptr<NameValueNode>(new NameValueNode(name, value) );
      return getChild(name);
   }
};


template <typename T>
class TypeNameValueNode : public NameValueNode {
   
   T _type;
   
   
protected:
   
   explicit TypeNameValueNode(const char *name, T type = T(), const char *value = "\0")
      : NameValueNode(name, value)
      , _type(type)
   {}
   
public:
   
   static NameValueNode* create(const char *name, T type = T(), const char *value = "\0") 
   {
      return new TypeNameValueNode<T>(name, type, value);
   }
   
   virtual void* getType() override final { return static_cast<T*>(&_type); }
   
};



template <typename T>
class NameValueTypeNode {
   
   /** 
    * if this node is an object _name is empty
    * if this node is an array element _name is the index in the array
    */
   const std::string _name;
   
   /** 
    *  _type is clang::QualType for input and output data trees
    *  _type is clang::FunctionDecl* for mock function trees
    */
   const T           _type;
   

   const std::string _value;
   
   
   std::map< std::string, std::shared_ptr<NameValueTypeNode<T> > > _children;

   
   bool _isNameInteger(void) const {
      if(_name.empty()) { return false; }
      char* p;
      strtol(_name.c_str(), &p, DECIMAL_BASE);
      return (*p == '\0');
   }
   
public:

   explicit NameValueTypeNode(const char *name, T type = T(), const char *value = "\0")
      : _name(name)
      , _type(type)
      , _value(value)
   {}

   ~NameValueTypeNode() {}

   const std::string &getName(void) const { return _name; }
   const std::string &getValue(void) const { return _value; }
   T getType(void) const { return _type; }
   unsigned int getNumChildern(void) { return _children.size(); }
   const std::map< std::string, std::shared_ptr<NameValueTypeNode<T> > >  &getChildren(void) const { return _children; }
   void addChild(std::shared_ptr<NameValueTypeNode<T> > child) { _children[child->_name] = child; }

   const bool isArrayElement(void) const { return _isNameInteger(); }
   
   const bool isArray(void) const { 
      if (_children.size() > 0 ) {
         return _children.begin()->second->_isNameInteger();
      }
      return false;
   }
   
   //const size_t getArraySize(void){ return _value.size(); }
   
   void setValue(const char *value ) { _value = value; }
   
   
   std::shared_ptr<NameValueTypeNode<T> > getChild(const char *name) const
   {
      auto iter = _children.find(name);
      if (iter == _children.end()) {
         std::cout << "ERROR: node " << _name << " has no child named '" << name << "'" << std::endl; 
         return nullptr;
      }
      return iter->second;
   }


   std::shared_ptr<NameValueTypeNode<T> > addChild(const char *name, T type, const char *value )
   {
      auto child = std::make_shared<NameValueTypeNode<T> >(name, type, value);
      _children[name] = child;

      return child;
   }


   const std::string & getIndexAsString(void)
   {
      if(_isNameInteger()){
         return _name;
      } else {
         throw std::logic_error(std::string("Error getIndexAsString: name is not an index\n"));
      }
   }

   
   int getIndex(void)
   {
      if(_isNameInteger()){
         return stoi(_name);
      } else {
         throw std::logic_error(std::string("Error getIndexAsString: name is not an index\n"));
      }
   }
   
};


#endif // _UTBuilder_NameValueTypeNode_h__

