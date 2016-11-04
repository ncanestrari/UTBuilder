#include "NameValueTypeNode.h"

#include "NameValueTypeVisitor.h"
#include "NameValueTypeActions.h"

// the order of static vars initialization is always respected for each translation unit 
const char* NameValueNode::_arrayIndex[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
const unsigned int NameValueNode::_arrayIndexSize = (sizeof((NameValueNode::_arrayIndex))/sizeof(*(NameValueNode::_arrayIndex)));

const char* NameValueNode::_arrayElementObject = "arrayElement";

// template <>
// const clang::QualType TypeNameValueNode<clang::QualType>::_defaultType = clang::QualType();
// 
// template <>
// const clang::FunctionDecl* const TypeNameValueNode<const clang::FunctionDecl*>::_defaultType = nullptr;


const clang::QualType QualTypeNode::_defaultType = clang::QualType();
const clang::FunctionDecl* const FunctionDeclNode::_defaultType = nullptr;


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


QualTypeNode::QualTypeNode(const QualTypeNode& other)
: NameValueNode(other)
, _type(other._type)
{}


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
