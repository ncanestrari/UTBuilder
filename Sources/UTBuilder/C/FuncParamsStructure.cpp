#include "FuncParamsStructure.h"

#include <clang/AST/Decl.h>

#include <assert.h> 


FuncParamsStruct::FuncParamsStruct() 
{}


FuncParamsStruct::~FuncParamsStruct()
{}


void FuncParamsStruct::init( const clang::FunctionDecl* funcDecl, const std::set<const clang::FunctionDecl*>& mockFuncs  )
{
   clear();

   _funcDecl = funcDecl;
   
   _inputTree.push_back( buildInputTree(funcDecl) );
   _inputTree.push_back( buildInputTree(funcDecl) );
   _outputTree.push_back( buildOutputTree(funcDecl) );
   _outputTree.push_back( buildOutputTree(funcDecl) );
   
   _mocksTree.push_back( buildMockFuncsTree(mockFuncs) );
   _mocksTree.push_back( buildMockFuncsTree(mockFuncs) );

}

void FuncParamsStruct::clear() 
{
   _funcDecl = nullptr;   
   
   _inputTree.clear();
   _outputTree.clear();
   _mocksTree.clear();

}

std::string FuncParamsStruct::getName(const unsigned int index) const 
{ 
   if ( index >= getSize() )
      return std::string();
   
   std::ostringstream sstream;
   sstream << _funcDecl->getNameAsString() << "_" << index;
   return sstream.str();
}

unsigned int FuncParamsStruct::getSize(void) const
{ 
   const unsigned int size = _outputTree.size(); 
   assert( size == _inputTree.size() );
   assert(size == _mocksTree.size() );
   return size;   
}

std::shared_ptr<NameValueTypeNode<clang::QualType> > FuncParamsStruct::buildInputTree( const clang::FunctionDecl* funcDecl)
{
      
   std::shared_ptr<NameValueTypeNode<clang::QualType> > root = std::make_shared<NameValueTypeNode<clang::QualType> >("input");
   
   if ( funcDecl->getNumParams() > 0 )
   {
      for ( auto field : funcDecl->params() )
      {
         root->addChild( field->getNameAsString().c_str(), field->getType(), ""  );
      }
   }
   
   return root;
}
   
std::shared_ptr<NameValueTypeNode<clang::QualType> > FuncParamsStruct::buildOutputTree( const clang::FunctionDecl* funcDecl)
{
   
   std::shared_ptr<NameValueTypeNode<clang::QualType> > root = buildInputTree( funcDecl );
   
//    only change name
   root->setName("output");
   
   // add return type
   root->addChild("retval", funcDecl->getReturnType(), "" );
   
   return root;
}
   
   
std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl*> > FuncParamsStruct::buildMockFuncsTree( const std::set<const clang::FunctionDecl*>& mockFuncs )
{
   std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl*> > root = std::make_shared<NameValueTypeNode<const clang::FunctionDecl*> >("mock-funcs-call");

   std::string value;
   for ( auto iter : mockFuncs )
   {
      value = iter->getNameAsString() + "[0]";
      root->addChild( iter->getNameAsString().c_str(), iter, value.c_str() );
   }
   
   return root;
}


void FuncParamsStruct::serializeTree( std::shared_ptr<NameValueTypeNode<clang::QualType> > tree, Json::Value& fieldItem)
{
   std::string comment;
   
   const std::string& keyName = tree->getName();
   
   if ( tree->getNumChildern() > 0 )
   {      
      for ( auto child : tree->getChildren() )
      {
         serializeTree(child.second, fieldItem[keyName] );
      }
      
      // add comment to json
      const clang::QualType qualType = tree->getType();
      if ( qualType.isNull() == false )
      {
         comment = "// struct " + qualType.getAsString();
         fieldItem[tree->getName()].setComment(comment.c_str(), comment.length(), Json::commentAfterOnSameLine);
      }
   }
   // don't set any value for already created objects ( input ,output and mock-funcs-call )
   else if ( fieldItem[keyName].isObject() == false ) {   
     
      fieldItem[keyName] = tree->getValue();
      
      // add comment to json
      const clang::QualType qualType = tree->getType();
      if ( qualType.isNull() == false )
      {
         const clang::QualType canonicalQualType = qualType->getCanonicalTypeInternal();
         comment = "// type: " + qualType.getAsString() + " (" + canonicalQualType.getAsString() + ") ";
         fieldItem[tree->getName()].setComment(comment.c_str(), comment.length(), Json::commentAfterOnSameLine);
      }         
   }
   
   return;
}


void FuncParamsStruct::serializeTree( std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl*> > tree, Json::Value& fieldItem)
{
   std::string comment; // = "// defined in mocks-json file";
   
   const std::string& keyName = tree->getName();
   
   if ( tree->getNumChildern() > 0 )
   {      
//       fieldItem[keyName].setComment(comment.c_str(), comment.length(), Json::commentBefore );
      
      for ( auto child : tree->getChildren() )
      {
         serializeTree(child.second, fieldItem[keyName] );
      }
   }
   else{      
      
      fieldItem[keyName] = tree->getValue();
   }
}


void FuncParamsStruct::serialize(Json::Value& jsonParent,  const bool withMocks )
{
   Json::Value jsonChild;   
   jsonChild["_name"] =  getName();
   jsonChild["content"] = Json::Value(Json::arrayValue);

   
   for (unsigned int i=0; i<_inputTree.size(); ++i) {
      
      // force "input" to be an object
      jsonChild["content"][i]["input"] = Json::Value(Json::objectValue);
      serializeTree( _inputTree[i], jsonChild["content"][i]);
   }
   
   for (unsigned int i=0; i<_outputTree.size(); ++i) {
      
      // force "output" to be an object
      jsonChild["content"][i]["output"] = Json::Value(Json::objectValue);
      serializeTree( _outputTree[i], jsonChild["content"][i]);
   }
      

   if ( withMocks ) {
      
      for (int i=0; i<_mocksTree.size(); ++i) {
         
            // force "mock-funcs-call" to be an object
            jsonChild["content"][i]["mock-funcs-call"] = Json::Value(Json::objectValue);
            serializeTree( _mocksTree[i], jsonChild["content"][i]);
      }
   }
   
         
//       add comment
   std::ostringstream comment;
   for (unsigned int i=0; i<_inputTree.size(); ++i) {
      comment.str(std::string());
      comment <<  "// " << getName() << "_test[" << i << "]";
      jsonChild["content"][i].setComment(comment.str().c_str(), comment.str().length(), Json::commentBefore);
   }

   
   jsonParent.append( jsonChild );
   
}


void FuncParamsStruct::deSerializeTree( std::shared_ptr<NameValueTypeNode<clang::QualType> > tree, const Json::Value& fieldItem) 
{
   const std::string& treeKeyName = tree->getName();
   Json::Value field = fieldItem.get(treeKeyName.c_str(), "");
   if ( field.isObject() == true ) {      
      for( Json::ValueIterator iter = field.begin() ; iter != field.end() ; iter++ ) {
         const std::string key = iter.key().asString();
         auto child = tree->getChild( key.c_str() );
         deSerializeTree( child, field ); 
      }
   } else{
      tree->setValue( field.asCString() );
   }
   
   return;
}


void FuncParamsStruct::deSerializeTree( std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl*> > tree, const Json::Value& fieldItem)
{
   
   if ( fieldItem.isObject() )
   {
      const std::string& treeKeyName = tree->getName();
      Json::Value field = fieldItem.get(treeKeyName.c_str(), "");
      
      if ( field.isObject() == true )
      {
         for( Json::ValueIterator iter = field.begin() ; iter != field.end() ; iter++ ) {
            const std::string key = iter.key().asString();
            auto child = tree->getChild( key.c_str() );
            deSerializeTree( child, *iter ); 
         }
      
      }
   }
   else
   {
      tree->setValue( fieldItem.asCString() );
   }
   
   return;
}

void FuncParamsStruct::deSerialize(const Json::Value& jsonRoot)
{
//    std::string encoding = jsonRoot.get("_name", "" ).asString();

   const Json::Value content = jsonRoot["content"];
   const unsigned int size = content.size();
   
   _inputTree.resize(size);
   _outputTree.resize(size);
   _mocksTree.resize(size);
   
   for ( int i = 0; i < size; ++i )
   {
      deSerializeTree( _inputTree[i], content[i] );
      deSerializeTree( _outputTree[i], content[i] );
      deSerializeTree( _mocksTree[i], content[i] );
   }
}

/*
void FuncParamsStruct::deSerializeTreeJson( std::shared_ptr<NameValueTypeNode<clang::QualType> > tree, const Json::Value& fieldItem) 
{
   const std::string& treeKeyName = tree->getName();
   Json::Value field = fieldItem.get(treeKeyName.c_str(), "");
   if ( field.isObject() == true ) {      
      for( Json::ValueIterator iter = field.begin() ; iter != field.end() ; iter++ ) {
         const std::string key = iter.key().asString();
         auto child = tree->getChild( key.c_str() );
         deSerializeTreeJson( child, field ); 
      }
   } else {
//       tree->setValue( field.asCString() );
      tree->addChild( treeKeyName.c_str(), tree->getType(), field.asString().c_str() );      
   }
}*/
   
   
std::shared_ptr<NameValueTypeNode<clang::QualType> > 
FuncParamsStruct::deSerializeTreeJson(const std::shared_ptr<NameValueTypeNode<clang::QualType> > referenceTree,
                                      const Json::Value& fieldItem )
{
      
   const std::string& treeKeyName = referenceTree->getName();
  
   std::shared_ptr<NameValueTypeNode<clang::QualType> > root = std::make_shared<NameValueTypeNode<clang::QualType> >(treeKeyName.c_str());
  
   root->setType( referenceTree->getType() );
   
   if ( fieldItem.isObject() == true ) { 
      //Json::Value field = fieldItem.get(treeKeyName.c_str(), "");
      for( Json::ValueConstIterator iter = fieldItem.begin() ; iter != fieldItem.end() ; iter++ ) {
      
         auto childReferenceTree = referenceTree->getChild(iter.key().asString().c_str());
         std::shared_ptr<NameValueTypeNode<clang::QualType> > child = deSerializeTreeJson(childReferenceTree, *iter );
         root->addChild( child );
      }
   }
   else {
      root->setValue( fieldItem.asString().c_str() );
   }
   
   return root;
}

std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl*> > 
FuncParamsStruct::deSerializeTreeJson( const std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl*> > referenceTree, const Json::Value& fieldItem)
{
   
   const std::string& treeKeyName = referenceTree->getName();
  
   std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl*> > root = std::make_shared<NameValueTypeNode<const clang::FunctionDecl*> >(treeKeyName.c_str());
  
   if ( fieldItem.isObject() == true ) { 
      //Json::Value field = fieldItem.get(treeKeyName.c_str(), "");
      for( Json::ValueConstIterator iter = fieldItem.begin() ; iter != fieldItem.end() ; iter++ ) {
      
         auto childReferenceTree = referenceTree->getChild(iter.key().asString().c_str());
         std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl*> > child = deSerializeTreeJson(childReferenceTree, *iter );
         root->addChild( child );
      }
   }
   else {
      root->setValue( fieldItem.asString().c_str() );
      root->setType( referenceTree->getType() );
   }
  
   return root;
}

void FuncParamsStruct::deSerializeJson( const FuncParamsStruct& funcParam, const Json::Value& jsonRoot)
{
//    std::string encoding = jsonRoot.get("_name", "" ).asString();

   const Json::Value content = jsonRoot["content"];
   const unsigned int size = content.size();
   
   _inputTree.resize(size);
   _outputTree.resize(size);
   _mocksTree.resize(size);
   
   for ( int i = 0; i < size; ++i )
   {
      _inputTree[i] = deSerializeTreeJson( funcParam._inputTree[i], content[i]["input"] );
      _outputTree[i]= deSerializeTreeJson( funcParam._outputTree[i], content[i]["output"] );
      _mocksTree[i] = deSerializeTreeJson( funcParam._mocksTree[i], content[i]["mock-funcs-call"] );
   }
}

    




static const char* getStructureField( std::ostringstream& os, const clang::QualType& qualType, const std::string& indent)
{
   const std::string newIndent = indent + indent;
   
   const clang::RecordType* structType = qualType->getAsStructureType();
         
//    if ( structType == nullptr )
//    {
   size_t pos = 0;

   std::string typestr = qualType.getUnqualifiedType().getAsString();
   pos = typestr.find("*", pos);
   while ( pos != std::string::npos ){
      typestr = typestr.erase(pos, 1);
      pos = typestr.find("*", pos);
   }
   return typestr.c_str();
//    }
      
   
//    if ( qualType->isUnionType() == false.
//       return;
/*
   const clang::RecordDecl* structDecl = structType->getDecl();
   
   os << "\n" << indent << "struct " << qualType.getAsString() << " { \n";
   
   for ( const auto field : structDecl->fields() ){

      const clang::QualType canonicalQualType = field->getType()->getCanonicalTypeInternal();

      os << newIndent << getStructureField( os, field->getType(), newIndent, false ) << "\t" << field->getNameAsString() << "; \n";// "<< canonicalQualType.getAsString() << "\n";
      //os << "   " << field->getType().getAsString() << "\t" << field->getNameAsString() << ";\n";
   }
         
   os << indent << "} "; // << qualType.getAsString() << ";\n\n"; // qualType->getCanonicalTypeInternal().getAsString()*/
}

void  FuncParamsStruct::writeAsStructure( std::ostringstream& os, const FuncParamsStruct& obj )
{
   const std::string indent = "   ";
 
   // write return type
//    os  << getStructureField( os, obj.getFunctionDecl()->getReturnType(), indent ) << " retval;\n\n"; 
         
//    const std::map< const std::string, const clang::DeclaratorDecl*> & args = obj.getFunctionDecl()->Params();
   
   os<< "\n";
   
   if ( obj.getNumParams() > 0 )
   {
      for ( auto field : obj.getFunctionDecl()->params() )
      {
         clang::QualType qualType = field->getType();
         const clang::QualType canonicalQualType = qualType->getCanonicalTypeInternal();

         os << indent << getStructureField( os, qualType, indent) << "\t" << field->getNameAsString() << "; \n";// "<< canonicalQualType.getAsString() << "\n"; 
      }
   }
   /*
   for ( auto field : args )
   {      
      clang::QualType qualType = field.second->getType();
      const clang::QualType canonicalQualType = qualType->getCanonicalTypeInternal();

      os << getStructureField( os, qualType ) << " " << qualType.getAsString() << "; // "<< canonicalQualType.getAsString() << "\n"; 
   }  */
   
   //os << "} " << obj.getName() << ";\n\n\n";
   
//    return os;
   return;
}


static const char* writeStructureValue( std::ostringstream& os, 
                                      const std::shared_ptr<NameValueTypeNode<clang::QualType> > tree, 
                                      const std::string& name,
                                      const std::string& indent )
{
//    const std::string newIndent = indent + indent;
   
   std::string structName = name + tree->getName();
  
   
   if ( tree->getNumChildern() > 0 )
   {      
      
      structName += ".";
      
      for ( auto child : tree->getChildren() )
      {
         writeStructureValue(os, child.second, structName, indent);
      }
   }
   else{
      if( tree->getValue() != "" ){
         os << indent << structName << " = " << tree->getValue() << ";\n";
      }
   }
   
   return "";
}

static const char* writeStructureComparison( std::ostringstream& os, 
                                             const std::shared_ptr<NameValueTypeNode<clang::QualType> > tree, 
                                             const std::string& name,
                                             const std::string& indent )
{
//    const std::string newIndent = indent + indent;
   
   std::string structName = name + tree->getName();
  
   
   if ( tree->getNumChildern() > 0 )
   {      
      
      structName += ".";
      
      for ( auto child : tree->getChildren() )
      {
         writeStructureComparison(os, child.second, structName, indent);
      }
   }
   else{
      if( tree->getValue() != "" ){
         os << indent << "EXPECT_EQ(" << structName << ", " << tree->getValue() << ");\n";
      }
   }
   
   return "";
}

void FuncParamsStruct::writeGoogleTest( std::ostringstream& os, const FuncParamsStruct& obj, const unsigned int i )
{
   const std::string indent = "   ";
   
   std::string structName(obj.getName());
   structName.append("_params");
   

//    input declaration
   os << indent << structName << " input;" << "\n"; 
   os << indent << "memset( &input, 0, sizeof(" << structName << ") );" << "\n\n";
   
//    mocks No recursion in mock tree 
   auto children = obj._mocksTree[i]->getChildren();
   for ( auto iter : children )
   {
      const std::string& value = iter.second->getValue();
      if (value != "")
      {
         os << indent << iter.first << "_fake.custom_fake = " << value << ";\n";
         //TODO fill the context with necessary manually written mocks fill the custom_fake with the content
      }
   }
   
   
//    output declaration
   const clang::QualType returnType = obj.getFunctionDecl()->getReturnType();
   const std::string& returnTypeString = returnType.getAsString();
   
   if ( returnTypeString != "void" )
   {
      os << indent << returnTypeString << " retval;" << "\n";
      //os << indent << "memset( &retval, 0, sizeof(" << returnTypeString << ") );" << "\n\n";
   }
   
   os << "// fill the input struct with json file values" << "\n";
   if ( obj.getNumParams() > 0 )
   {
      
      os << writeStructureValue( os, obj._inputTree[i], "", indent ) << "\n";
//       for ( auto field : obj.getFunctionDecl()->params() )
//       {
//          os  << indent << getStructureValue( os, field->getNameAsString(), structName) << "\n";         
//       }
      
   }
   
   
//    os << "// fill the return struct with json file values" << "\n";
//    writeStructureValue( os, obj._outputTree[i]->getChild("retval"), "", indent );
   
   
   os << "\n" <<  indent;

//    write function call
   if ( returnTypeString != "void" )
   {
      os << "retval = ";
   }
   
   
   os << obj.getName() << "(";
   
   if ( obj.getNumParams() > 0 )
   {
      const int numParms = obj.getFunctionDecl()->getNumParams();           

      const clang::ParmVarDecl* currentParam = obj.getFunctionDecl()->getParamDecl(0);
      os  << " input." << currentParam->getNameAsString();
      
      for ( int ii=1; ii<numParms; ++ii)
      {
         const clang::ParmVarDecl* currentParam = obj.getFunctionDecl()->getParamDecl(ii);
         if(currentParam->getOriginalType()->isAnyPointerType()){//need to be inproved
            os  << ", &input." << currentParam->getNameAsString();
         } else {
            os  << ", input." << currentParam->getNameAsString();
         }
         
      }      
   }
   os << ");\n\n";
   
   
   os << "// check conditions" << "\n";
   for( auto child : obj._outputTree[i]->getChildren() ){
      if(child.first=="retval"){
         writeStructureComparison( os, child.second, "", indent );
      } else {
         writeStructureComparison( os, child.second, "input.", indent );
      }
   }
   
   return;
}



/*
void FuncParamsStruct::writeAsStruct(void)
{
   
   std::cout << "typedef struct { \n\n";
   
   std::cout << FuncParamsStruct::getStructureField(_returnType.second) << " returnValue;\n\n"; 

   
//    if ( returnQualType->isStructureOrClassType() )
//    {
//       getStructureField( returnQualType );
//    }
//    
//    std::string returnType = returnQualType;
   
//       out.str("");
         
   for ( auto iter : _args )
   {      
      const clang::QualType qualType = iter.second->getType();
      const clang::QualType canonicalQualType = qualType->getCanonicalTypeInternal();

      std::cout << FuncParamsStruct::getStructureField( qualType ) << " " << qualType.getAsString() << " // "<< canonicalQualType.getAsString() << "\n"; 
   }  
   
   std::cout << "} " << _name << ";\n\n\n";
}


void FuncParamsStruct::traverseStructureField( Json::Value& value, const clang::QualType& qualType, const char* fieldName )
{
   const clang::RecordType* structType = qualType->getAsStructureType();
   if ( structType == nullptr )
   {
//       value["returnType"] = qualType.getUnqualifiedType().getAsString().c_str();
      const clang::QualType canonicalQualType = qualType->getCanonicalTypeInternal();
      
      const char* key; // = qualType.getAsString().c_str(); 
      std::string comment;
      
      if ( fieldName == "\0")
      {
         // must be a return type
         key = qualType.getAsString().c_str();
         comment = "// type: " + qualType.getAsString() + " (" + canonicalQualType.getAsString() + ") ";
         value[key] =  "";
         value[key].setComment(comment.c_str(), comment.length(), Json::commentAfterOnSameLine);
      }
      else{
         // must be a field
         key = fieldName;
         comment = "// type: " + qualType.getAsString() + " (" + canonicalQualType.getAsString() + ") ";
         value[key] =  ""; 
         value[key].setComment(comment.c_str(), comment.length(), Json::commentAfterOnSameLine);
         
         _input[fieldName] = std::make_pair(qualType, "0");
      }
      
      return;
   }
   
   const clang::RecordDecl* structDecl = structType->getDecl();

   value[qualType.getAsString()] = Json::Value::nullRef;
   
   for ( const auto field : structDecl->fields() ){
      
      FuncParamsStruct::traverseStructureField( value[qualType.getAsString()], field->getType(), field->getNameAsString().c_str() );
   }
         
   return;
}
*/


/*
void FuncParamsStruct::serializeJson(Json::Value& jsonParent)
{
   Json::Value jsonChild;
   Json::Value retItem;
   Json::Value fieldItem;
   Json::Value mockItem;
   
//    root["struct"] = _name.append("_params").c_str(); 
   //item["name"] =  _name.c_str();
   jsonChild["_name"] =  getName();
   
   
//    jsonRoot["struct"]["params"] = Json::Value::nullRef;
   
   if ( getNumParams() > 0 )
   {
      for ( auto field : _funcDecl->params() )
      {
         FuncParamsStruct::traverseStructureField(fieldItem, field->getType(), field->getNameAsString().c_str() );
      }
   }
//    else{
//       item["args"] = "void";
//    }   
   
   jsonChild["content"][0]["input"] = fieldItem;
   jsonChild["content"][1]["input"] = fieldItem;
   jsonChild["content"][0]["output"] = fieldItem;
   jsonChild["content"][1]["output"] = fieldItem;
   
   
   
   FuncParamsStruct::traverseStructureField(retItem, _output["return"].first ); //_returnType.second );
   
   //retItem["returnValue"] = FuncParamsStruct::getStructureField(_returnType);
   jsonChild["content"][0]["output"]["return"] = retItem;
   jsonChild["content"][1]["output"]["return"] = retItem;
   
   
   
   if ( _mocks.size() > 0 )
   {
      std::string key, val;
      std::string comment;
      
      for ( auto mock : _mocks )
      {
//          key = mock.second->getNameAsString();
         key = mock.first;
         val = key + "[0]";
         mockItem[key] = val;
      }
      
      jsonChild["content"][0]["mock-funcs-call"] = mockItem;
      jsonChild["content"][1]["mock-funcs-call"] = mockItem;
      
      const std::string mock_comment("// defined in the -mock.json file");
      jsonChild["content"][0]["mock-funcs-call"].setComment( mock_comment.c_str(), mock_comment.length(), Json::commentBefore);
      jsonChild["content"][1]["mock-funcs-call"].setComment( mock_comment.c_str(), mock_comment.length(), Json::commentBefore);
   }
      
    
   // add comments
   std::string baseComment = "// ";
   baseComment.append( getName() );
   baseComment.append("_test");
   std::string comment = baseComment + "[0]";
   jsonChild["content"][0].setComment(comment.c_str(), comment.length(), Json::commentBefore);
   comment =  baseComment + "[1]";
   jsonChild["content"][1].setComment(comment.c_str(), comment.length(), Json::commentBefore);
   
   jsonParent.append( jsonChild );
}
*/


// const char* FuncParamsStruct::getStructureField( const clang::QualType& qualType)
// {
//    
//    const clang::RecordType* structType = qualType->getAsStructureType();
//          
//    if ( structType == nullptr )
//    {
//       return qualType.getUnqualifiedType().getAsString().c_str();
//    }
//    
//    const clang::RecordDecl* structDecl = structType->getDecl();
//    
//    std::cout << "struct " << qualType.getAsString() << " { \n";
//    
//    for ( const auto field : structDecl->fields() ){
// 
//       clang::QualType qualType = field->getType();
//       std::cout << FuncParamsStruct::getStructureField( qualType ) << "\t" << field->getNameAsString() << "\n";
//       //out << "   " << field->getType().getAsString() << "\t" << field->getNameAsString() << ";\n";
//    }
//          
//    std::cout << "} "; // << qualType.getAsString() << ";\n\n"; // qualType->getCanonicalTypeInternal().getAsString()
//    
//  
//    return "";
// }


