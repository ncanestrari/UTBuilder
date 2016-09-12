#include "FuncParamsStructure.h"

#include <clang/AST/Decl.h>





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
    
void FuncParamsStruct::setName(const char* name)
{
   _name = name; 
   
}
    
const char* FuncParamsStruct::getName(void) const
{
   return _name.c_str();
}

void FuncParamsStruct::setReturnType( const clang::QualType& returnType) 
{ 
   _returnType = returnType; 
   
}
    
const clang::QualType& FuncParamsStruct::getReturnType(void) const
{ 
   return _returnType;
   
}
    
size_t FuncParamsStruct::getNumParams(clang::QualType qualType ) const
{ 
   return _args.size(); 
   
}
    
void FuncParamsStruct::addParam(const clang::QualType& qualType ) 
{ 
   _args.push_back(qualType); 
   
}
    
const clang::QualType FuncParamsStruct::getParam( const int index ) const
{ 
   return _args[index];
}
    

const std::vector<clang::QualType>& FuncParamsStruct::getParams() const
{ 
   return _args;   
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
      const clang::QualType canonicalQualType = iter->getCanonicalTypeInternal();

      std::cout << FuncParamsStruct::getStructureField( iter ) << " " << iter.getAsString() << " // "<< canonicalQualType.getAsString() << "\n"; 
   }  
   
   std::cout << "} " << _name << ";\n\n\n";
}

void FuncParamsStruct::serialize(void)
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

      std::cout << FuncParamsStruct::getStructureField( field->getType() ) << "\t" << field->getNameAsString() << "\n";
      //out << "   " << field->getType().getAsString() << "\t" << field->getNameAsString() << ";\n";
   }
         
   std::cout << "} "; // << qualType.getAsString() << ";\n\n"; // qualType->getCanonicalTypeInternal().getAsString()
   
 
   return "";
}


std::ostream& operator << (std::ostream& os, const FuncParamsStruct& obj)
{
   //os << "typedef struct { \n\n";   
   os << FuncParamsStruct::getStructureField( os, obj.getReturnType() ) << " returnValue;\n\n"; 
         
   const std::vector<clang::QualType>& args = obj.getParams();
   
   for ( auto iter : args )
   {      
      const clang::QualType canonicalQualType = iter->getCanonicalTypeInternal();

      os << FuncParamsStruct::getStructureField( os, iter ) << " " << iter.getAsString() << "; // "<< canonicalQualType.getAsString() << "\n"; 
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

