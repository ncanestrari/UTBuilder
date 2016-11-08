#include "NameValueTypeNode.h"

#include "NameValueTypeVisitor.h"
#include "NameValueTypeActions.h"

// the order of static vars initialization is always respected for each translation unit 
const char* NameValueNode::_arrayIndex[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
const unsigned int NameValueNode::_arrayIndexSize = (sizeof((NameValueNode::_arrayIndex))/sizeof(*(NameValueNode::_arrayIndex)));

const char* NameValueNode::_arrayElementObject = "arrayElement";




const clang::QualType QualTypeNode::_defaultType = clang::QualType();
const clang::FunctionDecl* const FunctionDeclNode::_defaultType = nullptr;


NameValueNode::NameValueNode(const NameValueNode& other) 
: _name(other._name)
, _value(other._value)
{      
}
   
   
NameValueNode::NameValueNode(const char *name, const char *value)
: _name(name)
, _value(value)
{}
   
   
bool NameValueNode::_isNameInteger(void) const 
{
   if(_name.empty()) { return false; }
   char* p;
   strtol(_name.c_str(), &p, DECIMAL_BASE);
   return (*p == '\0');
}
   
NameValueNode* NameValueNode::createValue(const char *name, const char *value) 
{
   return new NameValueNode(name, value);
}

NameValueNode* NameValueNode::createObject(const char *name)
{
   return new NameValueNode(name, "object");
}

NameValueNode* NameValueNode::createArray(const char *name) 
{
   return new NameValueNode(name, "array");
}

NameValueNode* NameValueNode::createArrayElement(unsigned int index, const char* val)
{
   if ( val == "") {
      val = NameValueNode::_arrayElementObject;
   }
   return new NameValueNode( std::to_string(index).c_str(), val );
}


NameValueNode* NameValueNode::clone(const char * value) const
{
   if (value == "") {
      return new NameValueNode(*this);
   }
   else {
      // clone but with different value
      return new NameValueNode( this->_name.c_str(), value);
   }
}   
   
   
void NameValueNode::visit( NodeVisitor* visitor) 
{
    visitor->process( this );
}
   
   
bool NameValueNode::execute( NodeAction* action)
{
   return action->run(this);
}

bool NameValueNode::execute( OneNodeVisitor* action )
{
   if ( action->run(this) ) {

      for (const auto& child : _children ) {
	  child.second.get()->execute(action);
      }
   }
}

int NameValueNode::getIndex(void) const
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

void NameValueNode::addChild(NameValueNode* child) 
{ 
   _children[child->_name] = std::unique_ptr<NameValueNode>(child); 
}
   
const NameValueNode* NameValueNode::getChild(const char *name) const
{
   auto iter = _children.find(name);
   if (iter == _children.end()) {
      std::cout << "ERROR: node " << _name << " has no child named '" << name << "'" << std::endl; 
      return nullptr;
   }
   return iter->second.get();
}
   
   
   

QualTypeNode::QualTypeNode(const QualTypeNode& other)
: NameValueNode(other)
, _type(other._type)
{}

QualTypeNode::QualTypeNode(const char *name, const clang::QualType& type, const char *value)
: NameValueNode(name, value)
, _type(type)
{}
   
   
QualTypeNode* QualTypeNode::clone(const char * value) const
{
   if (value == "") {
      return new QualTypeNode(*this);
   }
   else {
      // clone but with different value
      return new QualTypeNode( this->getName().c_str(), this->_type, value);
   }
}
   
void QualTypeNode::visit( NodeVisitor* visitor )
{
   visitor->process( this );
}

bool QualTypeNode::execute( NodeAction* action )
{
   return action->run(this);
}
   
   
bool QualTypeNode::execute( OneNodeVisitor* action )
{
   if ( action->run(this) ) {

      for (const auto& child : _children ) {
	  child.second.get()->execute(action);
      }
   }
}


FunctionDeclNode::FunctionDeclNode(const FunctionDeclNode& other)
: NameValueNode(other)
, _type(other._type)
{}

FunctionDeclNode::FunctionDeclNode(const char *name, const clang::FunctionDecl* type, const char *value)
: NameValueNode(name, value )
, _type(type)
{}
   
   
FunctionDeclNode* FunctionDeclNode::create(const char *name, const clang::FunctionDecl* type, const char *value)
{
   return new FunctionDeclNode(name, type, value);
}
   
FunctionDeclNode* FunctionDeclNode::clone(const char * value) const
{
   if (value == "") {
      return new FunctionDeclNode(*this);
   }
   else {
      return new FunctionDeclNode( this->getName().c_str(), this->_type, value);
   }
}
   
void FunctionDeclNode::visit( NodeVisitor* visitor )
{
   visitor->process( this );
}

bool FunctionDeclNode::execute( NodeAction* action )
{
   return action->run(this);
}
  
  
bool FunctionDeclNode::execute( OneNodeVisitor* action )
{
   
   if ( action->run(this) ) {

      for (const auto& child : _children ) {
	  child.second.get()->execute(action);
      }
   }
}
   
   
/*   
// template specialization for clang::QualType type
// QualType can be a structure and addChild is called recursevly for each struct field
template <>
NameValueNode* TypeNameValueNode<clang::QualType>::create(const char* name,  
                                                                 const clang::QualType& qualType, 
                                                                 const char* value ) 
{
   TypeNameValueNode<clang::QualType>* node = nullptr; // = new TypeNameValueNode<clang::QualType>(name, qualType, value);
   
   const clang::RecordType *structType = qualType->getAsStructureType();
   if (structType != nullptr) {
      // not useful , value can be "" for struct 
      const std::string structName = qualType.getAsString();
      
      node = new TypeNameValueNode<clang::QualType>(name, qualType, structName.c_str() );
  
      const clang::RecordDecl *structDecl = structType->getDecl();
      for (const auto field : structDecl->fields()) {
         auto* child = TypeNameValueNode<clang::QualType>::create(field->getNameAsString().c_str(), field->getType(), "");
         node->addChild(child);
      }
   }
   else {
    
      node = new TypeNameValueNode<clang::QualType>(name, qualType, value );
   }
   
   
   return node;
}
*/ 

QualTypeNode* QualTypeNode::create(const char* name, const clang::QualType& qualType, const char* value ) 
{
   QualTypeNode* node = nullptr; // = new TypeNameValueNode<clang::QualType>(name, qualType, value);
   
   const clang::RecordType *structType = qualType->getAsStructureType();
   if (structType != nullptr) {
      // not useful , value can be "" for struct 
      const std::string structName = qualType.getAsString();
      
      node = new QualTypeNode(name, qualType, structName.c_str() );
  
      const clang::RecordDecl *structDecl = structType->getDecl();
      for (const auto field : structDecl->fields()) {
         auto* child = QualTypeNode::create(field->getNameAsString().c_str(), field->getType(), "");
         node->addChild(child);
      }
   }
   else {
    
      node = new QualTypeNode(name, qualType, value );
   }
   
   
   return node;
}
   
// template specialization for clang::QualType type
// QualType can be a structure and addChild is called recursevly for each struct field
template <>
std::shared_ptr<NameValueTypeNode<clang::QualType> > NameValueTypeNode<clang::QualType>::addChild(const char *name, clang::QualType qualType, const char *value)
{
   std::shared_ptr<NameValueTypeNode<clang::QualType> > child;
   
   const clang::RecordType *structType = qualType->getAsStructureType();
   if (structType != nullptr) {
      // not useful , value can be "" for struct 
      const std::string structName = qualType.getAsString();
      
      child = std::make_shared<NameValueTypeNode<clang::QualType> >(name, qualType, structName.c_str() );
  
      const clang::RecordDecl *structDecl = structType->getDecl();
      for (const auto field : structDecl->fields()) {
         child->addChild(field->getNameAsString().c_str(), field->getType(), "");
      }
   }
   else {
    
      child = std::make_shared<NameValueTypeNode<clang::QualType> >(name, qualType, value );
   }
   
   _children[name] = child;
   return child;
}
