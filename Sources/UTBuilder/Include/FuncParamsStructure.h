#ifndef _UTBuilder_FuncParamsStructure_h__
#define _UTBuilder_FuncParamsStructure_h__

#include <clang/AST/Type.h>

#include <string>
#include <vector>
#include <iostream>

/*
namespace clang {
  class QualType;
}
*/


class FuncParamsStruct
{

public:
    FuncParamsStruct();
    ~FuncParamsStruct();
    
    void clear();
    
    void setName(const char* name);
    
    const char* getName(void) const;
    
    
    void setReturnType( const clang::QualType& returnType);
    
    const clang::QualType& getReturnType(void) const;
    
    size_t getNumParams(clang::QualType qualType ) const;
    
    void addParam(const clang::QualType& qualType );
    
    const clang::QualType getParam( const int index ) const;
    
    
    void serialize(void);
    
    void writeAsStruct(void);
    
private:
   
   const std::vector<clang::QualType>& getParams(void) const;
   
   static const char* getStructureField( const clang::QualType& qualType);
   
   static const char* getStructureField( std::ostream& os, const clang::QualType& qualType);
   
   
   std::string _name;
   
   clang::QualType _returnType;
   
   std::vector<clang::QualType> _args;
   
   friend std::ostream& operator << (std::ostream& os, const FuncParamsStruct& obj);
   
};


std::ostream& operator << (std::ostream& os, const FuncParamsStruct& obj);



#endif // _UTBuilder_FuncParamsStructure_h__

