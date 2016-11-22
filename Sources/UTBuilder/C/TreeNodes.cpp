#include "TreeNodes.h"

// #include "NameValueTypeVisitor.h"
// #include "NameValueTypeActions.h"

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
   if ( val == nullptr ) {
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
   

QualTypeNode* QualTypeNode::create(const char* name, const clang::QualType& qualType, const char* value ) 
{
   QualTypeNode* node = nullptr;
   
   const clang::RecordType *structType = qualType->getAsStructureType();
   if (structType != nullptr) {
      // not useful , value can be "" for struct 
      const std::string structName = qualType.getAsString();
      
      node = new QualTypeNode(name, qualType, structName.c_str() );
  
      const clang::RecordDecl *structDecl = structType->getDecl();
      for (const auto& field : structDecl->fields()) {
         auto* child = QualTypeNode::create(field->getNameAsString().c_str(), field->getType(), "");
         node->addChild(child);
      }
   }
   else {
    
      node = new QualTypeNode(name, qualType, value );
   }
   
   return node;
}
