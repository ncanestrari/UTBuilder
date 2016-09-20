#include "FuncParamsStructure.h"

#include <clang/AST/Decl.h>




FuncParamsStruct::FuncParamsStruct() 
{}


FuncParamsStruct::~FuncParamsStruct()
{}


void FuncParamsStruct::init( const clang::FunctionDecl* funcDecl, const std::set<const clang::FunctionDecl*>& mockFuncs )
{
   clear();

   _funcDecl = funcDecl;
   
   _inputTree.push_back( buildInputTree(funcDecl) );
   _inputTree.push_back( buildInputTree(funcDecl) );
   _outputTree.push_back( buildOutputTree(funcDecl) );
   _outputTree.push_back( buildOutputTree(funcDecl) );
   
   if ( mockFuncs.size() > 0 ) {
      
      _mocksTree.push_back( buildMockFuncsTree( mockFuncs ) );
      _mocksTree.push_back( buildMockFuncsTree( mockFuncs ) );
   }
   
}

void FuncParamsStruct::clear() 
{
   _funcDecl = nullptr;   
   
   _inputTree.clear();
   _outputTree.clear();
   _mocksTree.clear();

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
   root->_name = "output";
   
   // add return type
   root->addChild("return", funcDecl->getReturnType(), "" );
   
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
   else{      
      
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


void FuncParamsStruct::serializeJson(Json::Value& jsonParent)
{
   Json::Value jsonChild;
//    Json::Value retItem;
//    Json::Value fieldItem;
//    Json::Value mockItem;
   
   jsonChild["_name"] =  getName();
   jsonChild["content"] = Json::Value::nullRef;

   
   for (unsigned int i=0; i<_inputTree.size(); ++i) {
      
      serializeTree( _inputTree[i], jsonChild["content"][i]);
   }
   
   for (unsigned int i=0; i<_outputTree.size(); ++i) {
      serializeTree( _outputTree[i], jsonChild["content"][i]);
   }
      
//    serializeTree( _inputTree, jsonChild["content"]);
//    serializeTree( _outputTree, jsonChild["content"]);

   
   if ( _mocksTree.size() > 0 )
   {
      for (int i=0; i<_mocksTree.size(); ++i) {
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
   const std::string jsonKeyValue = field.asString();
   
   // check json file correctness
   if ( jsonKeyValue == "" )
   {
      return;
   }
   
   const unsigned int numChildren = tree->getNumChildern();
   if ( tree->getNumChildern() > 0 )
   {      
       // check json file correctness
      const unsigned int size = fieldItem.size();
      if ( numChildren != size )
      {
         return;
      }
   
      for ( auto child : tree->getChildren() )
      {
         deSerializeTree(child.second, field);
      }
      
      // add comment to json
//       const clang::QualType qualType = tree->getType();
//       if ( qualType.isNull() == false )
//       {
//          comment = "// struct " + qualType.getAsString();
//          fieldItem[tree->getName()].setComment(comment.c_str(), comment.length(), Json::commentAfterOnSameLine);
//       }
   }
   else{      
      
      tree->setValue( jsonKeyValue.c_str() );
      
      // add comment to json
/*      const clang::QualType qualType = tree->getType();
      if ( qualType.isNull() == false )
      {
         const clang::QualType canonicalQualType = qualType->getCanonicalTypeInternal();
         comment = "// type: " + qualType.getAsString() + " (" + canonicalQualType.getAsString() + ") ";
         fieldItem[tree->getName()].setComment(comment.c_str(), comment.length(), Json::commentAfterOnSameLine);
      }*/         
   }
   
}

void FuncParamsStruct::deSerializeJson(Json::Value& jsonRoot)
{
   std::string encoding = jsonRoot.get("_name", "" ).asString();

   const Json::Value content = jsonRoot["content"];
   const unsigned int size = content.size();
   
   _inputTree.resize(size);
   _outputTree.resize(size);
   _mocksTree.resize(size);
   
   for ( int i = 0; i < size; ++i )
   {
      deSerializeTree( _inputTree[i], content[i] );
      deSerializeTree( _outputTree[i], content[i] );
//       deSerializeTree( _mocksTree, content[i] );
   }
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


// std::ostream& operator << (std::ostream& os, const FuncParamsStruct& obj)
// {
//    
//    //os << "typedef struct { \n\n";   
//    os << FuncParamsStruct::getStructureField( os, obj.getReturnType() ) << " returnValue;\n\n"; 
//          
//    const std::map< const std::string, const clang::DeclaratorDecl*> & args = obj.getParams();
//    
//    for ( auto field : args )
//    {      
//       clang::QualType qualType = field.second->getType();
//       const clang::QualType canonicalQualType = qualType->getCanonicalTypeInternal();
// 
//       os << FuncParamsStruct::getStructureField( os, qualType ) << " " << qualType.getAsString() << "; // "<< canonicalQualType.getAsString() << "\n"; 
//    }  
//    
//    //os << "} " << obj.getName() << ";\n\n\n";
//    
//    return os;
// }


// const char* FuncParamsStruct::getStructureField( std::ostream& os, const clang::QualType& qualType)
// {
//    
//    const clang::RecordType* structType = qualType->getAsStructureType();
//          
//    if ( structType == nullptr )
//    {
//       return qualType.getUnqualifiedType().getAsString().c_str();
//    }
//    
//    
// //    if ( qualType->isUnionType() == false.
// //       return;
// 
//    const clang::RecordDecl* structDecl = structType->getDecl();
//    
//    os << "struct " << qualType.getAsString() << " { \n";
//    
//    for ( const auto field : structDecl->fields() ){
// 
//       const clang::QualType canonicalQualType = field->getType()->getCanonicalTypeInternal();
//       
//       os << FuncParamsStruct::getStructureField( field->getType() ) << "\t" << field->getNameAsString() << "; // "<< canonicalQualType.getAsString() << "\n";
//       //os << "   " << field->getType().getAsString() << "\t" << field->getNameAsString() << ";\n";
//    }
//          
//    os << "} "; // << qualType.getAsString() << ";\n\n"; // qualType->getCanonicalTypeInternal().getAsString()
//    
//  
//    return "";
// }

