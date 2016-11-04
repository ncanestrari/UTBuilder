
#ifndef _UTBuilder_NameValueTypeNode_h__
#define _UTBuilder_NameValueTypeNode_h__

#include <clang/AST/Decl.h>
#include <map>
#include <string>
#include <iostream>
#include <exception>

#define DECIMAL_BASE (10)


class NodeVisitor;
class NodeAction;
class OneNodeVisitor;


class NameValueNode {

   const std::string _name;
   const std::string _value;   
   
protected:
   
   std::map< std::string, std::unique_ptr<NameValueNode> > _children;
   
   NameValueNode(const char *name, const char *value)
      : _name(name)
      , _value(value)
   {}
   
   NameValueNode() = delete;
   
   NameValueNode(const NameValueNode& other) 
   : _name(other._name)
   , _value(other._value)
   {      
   }
   
   bool _isNameInteger(void) const {
      if(_name.empty()) { return false; }
      char* p;
      strtol(_name.c_str(), &p, DECIMAL_BASE);
      return (*p == '\0');
   }
   
public:

  
   static const char* _arrayIndex[];
   static const unsigned int _arrayIndexSize; 

   static const char* _arrayElementObject;
   
   virtual ~NameValueNode() {}
   
   static NameValueNode* createValue(const char *name, const char *value = "") {
      return new NameValueNode(name, value);
   }
   
   static NameValueNode* createObject(const char *name) {
      return new NameValueNode(name, "object");
   }
   
   static NameValueNode* createArray(const char *name) {
      return new NameValueNode(name, "array");
   }
   
   static NameValueNode* createArrayElement(unsigned int index, const char* val = "") {

      if ( val == "") {
         val = NameValueNode::_arrayElementObject;
      }
      return new NameValueNode( std::to_string(index).c_str(), val );
   }
   

   virtual NameValueNode* clone()
   {
      return new NameValueNode(*this);
   }
   
   
   virtual void visit( NodeVisitor* visitor );
   
//    virtual void visit( NodeVisitor* visitor, NameValueNode* other);
   
   virtual bool execute( NodeAction* action );
   

   virtual bool execute( OneNodeVisitor* action );
   
   
   const std::string &getName(void) const { return _name; }
   const std::string &getValue(void) const { return _value; }
   

   virtual const void* getType() const { return nullptr; }
   
   
   bool isArray() const { return (_value == "array" ); }
   bool isObject() const { return (_value == "object" );}
   
   const bool isArrayElement(void) const { return (_value == NameValueNode::_arrayElementObject || _isNameInteger() ); }
   const bool isArrayElementObject(void) const { return (_value == NameValueNode::_arrayElementObject ); }
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
   const std::map< std::string, std::unique_ptr<NameValueNode> >& getChildren(void) const { return _children; }
   void addChild(NameValueNode* child) 
   { 
     _children[child->_name] = std::unique_ptr<NameValueNode>(child); 
   }
   
   const NameValueNode* getChild(const char *name) const
   {
      auto iter = _children.find(name);
      if (iter == _children.end()) {
         std::cout << "ERROR: node " << _name << " has no child named '" << name << "'" << std::endl; 
         return nullptr;
      }
      return iter->second.get();
   }

  /*
   const NameValueNode* addChild(const char *name, const char *value )
   {
      _children[name] = std::unique_ptr<NameValueNode>(new NameValueNode(name, value) );
      return getChild(name);
   }
   */
};



/*
template <typename T>
class TypeNameValueNode : public NameValueNode {
   
   // switch to mutable to avoid  a const_cast<>() in function void* getType() const and many other const in static_cast<> 
//    mutable T _type;
   T _type;
   
protected:
   
   explicit TypeNameValueNode(const char *name, const T& type, const char *value = "\0")
      : NameValueNode(name, value)
      , _type(type)
   {}
   
public:
   
   static NameValueNode* create(const char *name, const T& type = TypeNameValueNode<T>::_defaultType, const char *value = "\0") 
   {
      return new TypeNameValueNode<T>(name, type, value);
   }
   
   virtual const void* getType() const override final { return &_type; }
   
   static const T _defaultType;
   
};*/


class QualTypeNode : public NameValueNode 
{
   
   // switch to mutable to avoid  a const_cast<>() in function void* getType() const and many other const in static_cast<> 
//    mutable T _type;
   clang::QualType _type;
   
protected:
   
   explicit QualTypeNode(const char *name, const clang::QualType& type, const char *value = "\0")
      : NameValueNode(name, value)
      , _type(type)
   {}
   
   QualTypeNode() = delete;
   
   QualTypeNode(const QualTypeNode&);
   
   
public:
   
   virtual ~QualTypeNode() {}
  
   static QualTypeNode* create(const char *name, const clang::QualType& type = QualTypeNode::_defaultType, const char *value = "\0") ;
//    {
//       return new QualTypeNode(name, type, value);
//    }
   
   virtual QualTypeNode* clone() override final
   {
      return new QualTypeNode(*this);
   }
   
   virtual const void* getType() const override final { return &_type; }
   
   virtual void visit( NodeVisitor* visitor ) override final;
   
   virtual bool execute( NodeAction* action ) override final;
   
   virtual bool execute( OneNodeVisitor* action ) override final;
   
   static const clang::QualType _defaultType;
   
};


class FunctionDeclNode : public NameValueNode
{
  
   const clang::FunctionDecl* _type;
   
protected:
  
   explicit FunctionDeclNode(const char *name, const clang::FunctionDecl* type, const char *value = "\0")
      : NameValueNode(name, value )
      , _type(type)
   {}
   
   FunctionDeclNode() = delete;
   
   FunctionDeclNode(const FunctionDeclNode& other);
   
public:
  
   static const clang::FunctionDecl* const _defaultType;
  
   virtual ~FunctionDeclNode() {}
   
   virtual const void* getType() const override final{ return _type; }
   
   static FunctionDeclNode* create(const char *name, const clang::FunctionDecl* type = FunctionDeclNode::_defaultType, const char *value = "\0")
   {
      return new FunctionDeclNode(name, type, value);
   }
   
   virtual FunctionDeclNode* clone() override final
   {
      return new FunctionDeclNode(*this);
   }
   
   
   virtual void visit( NodeVisitor* visitor ) override final;
   
   virtual bool execute( NodeAction* action ) override final;
   
   virtual bool execute( OneNodeVisitor* action ) override final;
   
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

