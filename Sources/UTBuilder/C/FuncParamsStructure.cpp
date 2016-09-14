#include "FuncParamsStructure.h"

#include <clang/AST/Decl.h>

// #include <json/json.h>



FuncParamsStruct::FuncParamsStruct() 
{}

FuncParamsStruct::~FuncParamsStruct()
{}

void FuncParamsStruct::clear() 
{
   _name = "";   
   _returnType = clang::QualType();
   _args.clear();
}


void FuncParamsStruct::writeAsStruct(void)
{
   
   std::cout << "typedef struct { \n\n";
   
   std::cout << FuncParamsStruct::getStructureField(_returnType) << " returnValue;\n\n"; 

   /*
   if ( returnQualType->isStructureOrClassType() )
   {
      getStructureField( returnQualType );
   }
   
   std::string returnType = returnQualType;
   */
//       out.str("");
         
   for ( auto iter : _args )
   {      
      const clang::QualType qualType = iter->getType();
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
         comment = "// type" + qualType.getAsString() + " (" + canonicalQualType.getAsString() + ") ";
         value[key] =  "";
         value[key].setComment(comment.c_str(), comment.length(), Json::commentAfterOnSameLine);
      }
      else{
         // must be a field
         key = fieldName;
         comment = "// type" + qualType.getAsString() + " (" + canonicalQualType.getAsString() + ") ";
         value[key] =  ""; 
         value[key].setComment(comment.c_str(), comment.length(), Json::commentAfterOnSameLine);
      }
      
      return;
   }
   
   const clang::RecordDecl* structDecl = structType->getDecl();

   value[qualType.getAsString()] = Json::Value::nullRef;
   
   for ( const auto field : structDecl->fields() ){

//       clang::QualType qualType = field->getType();
//       std::cout << FuncParamsStruct::getStructureField( qualType ) << "\t" << field->getNameAsString() << "\n";
      //out << "   " << field->getType().getAsString() << "\t" << field->getNameAsString() << ";\n";
      
      FuncParamsStruct::traverseStructureField( value[qualType.getAsString()], field->getType(), field->getNameAsString().c_str() );
   }
         
   return;
}

void FuncParamsStruct::serializeJson(Json::Value& jsonParent)
{
   Json::Value jsonChild;
   Json::Value retItem;
   Json::Value item;
   Json::Value mockItem;
   
//    root["struct"] = _name.append("_params").c_str(); 
   //item["name"] =  _name.c_str();
   jsonChild["name"] =  _name.c_str();
   
   
   FuncParamsStruct::traverseStructureField(retItem, _returnType );
   
   //retItem["returnValue"] = FuncParamsStruct::getStructureField(_returnType);
   jsonChild["tests"][0]["return"] = retItem;
   jsonChild["tests"][1]["return"] = retItem;
   
//    jsonRoot["struct"]["params"] = Json::Value::nullRef;
   
   for ( auto field : _args )
   {
      FuncParamsStruct::traverseStructureField(item, field->getType(), field->getNameAsString().c_str() );
   }
   
   jsonChild["tests"][0]["args"] = item;
   jsonChild["tests"][1]["args"] = item;

   if ( _mockFunctions.size() > 0 )
   {
      std::string key, val;
      std::string comment;
      
      for ( auto mock : _mockFunctions )
      {
         key = mock->getNameAsString();
         val = key + "[0]";
         mockItem[key] = val;
      }
      
      jsonChild["tests"][0]["mock-funcs"] = mockItem;
      jsonChild["tests"][1]["mock-funcs"] = mockItem;
      
      const std::string mock_comment("// defined in the -mock.json file");
      jsonChild["tests"][0]["mock-funcs"].setComment( mock_comment.c_str(), mock_comment.length(), Json::commentBefore);
      jsonChild["tests"][1]["mock-funcs"].setComment( mock_comment.c_str(), mock_comment.length(), Json::commentBefore);
   }
      
    
      
   std::string baseComment = "// ";
   baseComment.append( _name.c_str() );
   baseComment.append("_test");
   std::string comment = baseComment + "[0]";
   jsonChild["tests"][0].setComment(comment.c_str(), comment.length(), Json::commentBefore);
   comment =  baseComment + "[1]";
   jsonChild["tests"][1].setComment(comment.c_str(), comment.length(), Json::commentBefore);
   
   jsonParent.append( jsonChild );
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


std::ostream& operator << (std::ostream& os, const FuncParamsStruct& obj)
{
   
   //os << "typedef struct { \n\n";   
   os << FuncParamsStruct::getStructureField( os, obj.getReturnType() ) << " returnValue;\n\n"; 
         
   const std::vector<const clang::DeclaratorDecl*>& args = obj.getParams();
   
   for ( auto field : args )
   {      
      clang::QualType qualType = field->getType();
      const clang::QualType canonicalQualType = qualType->getCanonicalTypeInternal();

      os << FuncParamsStruct::getStructureField( os, qualType ) << " " << qualType.getAsString() << "; // "<< canonicalQualType.getAsString() << "\n"; 
   }  
   
   //os << "} " << obj.getName() << ";\n\n\n";
   
   return os;
}


const char* FuncParamsStruct::getStructureField( std::ostream& os, const clang::QualType& qualType)
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
   
   os << "struct " << qualType.getAsString() << " { \n";
   
   for ( const auto field : structDecl->fields() ){

      const clang::QualType canonicalQualType = field->getType()->getCanonicalTypeInternal();
      
      os << FuncParamsStruct::getStructureField( field->getType() ) << "\t" << field->getNameAsString() << "; // "<< canonicalQualType.getAsString() << "\n";
      //os << "   " << field->getType().getAsString() << "\t" << field->getNameAsString() << ";\n";
   }
         
   os << "} "; // << qualType.getAsString() << ";\n\n"; // qualType->getCanonicalTypeInternal().getAsString()
   
 
   return "";
}

