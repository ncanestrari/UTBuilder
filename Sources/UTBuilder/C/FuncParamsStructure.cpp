#include "FuncParamsStructure.h"

#include <clang/AST/Decl.h>

// #include <json/json.h>



FuncParamsStruct::FuncParamsStruct() 
{}

FuncParamsStruct::~FuncParamsStruct()
{}

void FuncParamsStruct::init( const clang::FunctionDecl* funcDecl, const std::set<const clang::FunctionDecl*>& mockFuncs )
{
   clear();

   _funcDecl = funcDecl;
   
//    setName(funcDecl->getNameAsString().c_str());
   setReturnType( funcDecl->getReturnType() );

   const int numParms = funcDecl->getNumParams();           
   for ( int i=0; i<numParms; ++i)
   {
      addParam( funcDecl->getParamDecl(i), "" );
   }
   
   if ( mockFuncs.size() > 0 )
   {
//       const std::set<const clang::FunctionDecl*>& declSet = mockFuncs.second;
      
      for ( auto iter : mockFuncs )
      {
         addMockFunction(iter, iter->getNameAsString() + "[0]" );
      }
   }
   
   
   _inputTree = buildInputTree(funcDecl);
   
   _outputTree = buildOutputTree(funcDecl);
}

void FuncParamsStruct::clear() 
{
   _funcDecl = nullptr;
//    _name = "";      
   
   _input.clear();
   _output.clear();
   _mocks.clear();
}


void buildTree(std::shared_ptr<NameValueNode> node, const clang::QualType& qualType, const char* fieldName )
{
}

std::shared_ptr<NameValueNode> FuncParamsStruct::buildInputTree( const clang::FunctionDecl* funcDecl)
{
      
   std::shared_ptr<NameValueNode> root = std::make_shared<NameValueNode>("input");
   
   if ( funcDecl->getNumParams() > 0 )
   {
      for ( auto field : funcDecl->params() )
      {
         root->addChild( field->getNameAsString().c_str(), field->getType(), ""  );
      }
   }
   
   return root;
}
   
std::shared_ptr<NameValueNode> FuncParamsStruct::buildOutputTree( const clang::FunctionDecl* funcDecl)
{
   
   std::shared_ptr<NameValueNode> root = buildInputTree( funcDecl );
   
//    only change name
   root->_name = "output";
   
   // add return type
   root->addChild("return", funcDecl->getReturnType() );
   
   return root;
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
*/

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


void FuncParamsStruct::deSerializeJson(Json::Value& jsonRoot)
{
}

const char* FuncParamsStruct::getStructureField( const clang::QualType& qualType)
{
   
   const clang::RecordType* structType = qualType->getAsStructureType();
         
   if ( structType == nullptr )
   {
      return qualType.getUnqualifiedType().getAsString().c_str();
   }
   
   /*
   if ( qualType->isUnionType() == false.
      return;
*/
   const clang::RecordDecl* structDecl = structType->getDecl();
   
   std::cout << "struct " << qualType.getAsString() << " { \n";
   
   for ( const auto field : structDecl->fields() ){

      clang::QualType qualType = field->getType();
      std::cout << FuncParamsStruct::getStructureField( qualType ) << "\t" << field->getNameAsString() << "\n";
      //out << "   " << field->getType().getAsString() << "\t" << field->getNameAsString() << ";\n";
   }
         
   std::cout << "} "; // << qualType.getAsString() << ";\n\n"; // qualType->getCanonicalTypeInternal().getAsString()
   
 
   return "";
}


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

