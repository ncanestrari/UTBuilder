#ifndef _UTBuilder_FuncParamsStructure_h__
#define _UTBuilder_FuncParamsStructure_h__

// #include <clang/AST/Type.h>
#include <clang/AST/Decl.h>

#include <string>
#include <vector>
#include <iostream>

#include <json/json.h>

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
    
    void setName(const char* name) { _name = name; }
    
    const char* getName(void) const { return _name.c_str(); }
    
    
    void setReturnType( const clang::QualType& returnType) { _returnType = returnType; }
    
    const clang::QualType& getReturnType(void) const { return _returnType; }
    
    size_t getNumParams(void) const { return _args.size(); }
    
    void addParam(const clang::DeclaratorDecl* fieldDecl ) { _args.push_back(fieldDecl); }
    
    const clang::DeclaratorDecl* getParam( const int index ) const { return _args[index]; }
    
    
    void serialize(void);
    
    void writeAsStruct(void);
    
private:
   
   const std::vector<const clang::DeclaratorDecl*>& getParams(void) const { return _args; }
   
   static const char* getStructureField( const clang::QualType& qualType);
   
   static const char* getStructureField( std::ostream& os, const clang::QualType& qualType);
   
   static void getStructureField( Json::Value& value, const clang::QualType& qualType, const char* fieldName = "\0" );
   
   
   std::string _name;
   
   clang::QualType _returnType;
   
   std::vector<const clang::DeclaratorDecl*> _args;
   
   friend std::ostream& operator << (std::ostream& os, const FuncParamsStruct& obj);
   
};


std::ostream& operator << (std::ostream& os, const FuncParamsStruct& obj);



#endif // _UTBuilder_FuncParamsStructure_h__

