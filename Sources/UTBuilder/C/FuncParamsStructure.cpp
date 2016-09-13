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


void FuncParamsStruct::getStructureField( Json::Value& value, const clang::QualType& qualType, const char* fieldName )
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
      
      FuncParamsStruct::getStructureField( value[qualType.getAsString()], field->getType(), field->getNameAsString().c_str() );
   }
         
   return;
}

void FuncParamsStruct::serialize(void)
{
   Json::Value root;
   Json::Value retItem;
   Json::Value item;
   
//    root["struct"] = _name.append("_params").c_str(); 
   //item["name"] =  _name.c_str();
   root["functions"]["name"] =  _name.c_str();
   
   
   FuncParamsStruct::getStructureField(retItem, _returnType );
   
   //retItem["returnValue"] = FuncParamsStruct::getStructureField(_returnType);
   root["functions"]["tests"][0]["output"] = retItem;
   root["functions"]["tests"][1]["output"] = retItem;
  
   
   //root["struct"]["params"] = Json::Value::nullRef;
   
   const char* paramstring = "param";
   char counterValue[16];
   
   std::string valStr;
   std::string comment;
   
//    root["struct"]["params"] = Json::Value::nullRef;
   
   for ( int i=0; i< _args.size(); ++i )
   {
      FuncParamsStruct::getStructureField(item, _args[i]->getType(), _args[i]->getNameAsString().c_str() );
      /*
      const clang::QualType qualType = _args[i]->getType();
      const clang::QualType canonicalQualType = qualType->getCanonicalTypeInternal();
      sprintf(counterValue,"%d",i);
      valStr = paramstring;
      valStr.append(counterValue);
      item[_args[i]->getNameAsString()] = "";
      comment = "// " + valStr + ": type" + qualType.getAsString() + " (" + canonicalQualType.getAsString() + ") ";
      item[_args[i]->getNameAsString()].setComment(comment.c_str(), comment.length(), Json::commentAfterOnSameLine );
      */
   }
  
  root["functions"]["tests"][0]["input"] = item;
  root["functions"]["tests"][1]["input"] = item;
  
  std::cout << root;
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

