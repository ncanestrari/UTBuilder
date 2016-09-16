#ifndef _UTBuilder_FuncParamsStructure_h__
#define _UTBuilder_FuncParamsStructure_h__

// #include <clang/AST/Type.h>
#include <clang/AST/Decl.h>

#include <string>
#include <set>
#include <map>
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
    
    void init( const clang::FunctionDecl* funcDecl, const std::set<const clang::FunctionDecl*>& mockFuncs = std::set<const clang::FunctionDecl*>() );   
    
    
    const char* getName(void) const { return _funcDecl->getNameAsString().c_str(); }    
 
 // fix this: it's not safe if the find("return") is not found
    const std::pair< clang::QualType, std::string>& getReturnType(void) const 
      { return _output.find("return")->second; }
    
    
    size_t getNumParams(void) const { return _funcDecl->getNumParams(); }
    
//     const clang::DeclaratorDecl* getParam( const int index ) const { return _args[index]; }
    const std::map< std::string, std::pair< clang::QualType, std::string> >::iterator getInputParam( const std::string& name ) 
    { return _input.find(name); }
    
    const std::map< std::string, std::pair< clang::QualType, std::string> >::iterator getOutputParam( const std::string& name ) 
    { return _output.find(name); }
    
    
    void serializeJson(Json::Value& jsonRoot);
    
    void deSerializeJson(Json::Value& jsonRoot);
    
    void writeAsStruct(void);
    
private:
   
   void clear(void);
   
//    void setName(const char* name) { _name = name; }
   
   void setReturnType( const clang::QualType& returnType) 
   { 
      _output["return"] = std::make_pair( returnType, returnType.getAsString() );
   }
   
   void addParam(const clang::DeclaratorDecl* decl, const std::string& value )
    {
//        _args.push_back(decl); 
       _input[decl->getNameAsString()] = std::make_pair( decl->getType(), value );
       _output[decl->getNameAsString()] = std::make_pair( decl->getType(), value );
   }
   
   void addMockFunction(const clang::DeclaratorDecl* decl, const std::string& value ) 
    {
//        _mockFunctions[decl->getNameAsString()] = decl;
       _mocks[decl->getNameAsString()] = std::make_pair( decl, value );       
   }
   
   
   //const std::map< const std::string, const clang::DeclaratorDecl* >& getParams(void) const { return _args; }
   
   static const char* getStructureField( const clang::QualType& qualType);
   
   static const char* getStructureField( std::ostream& os, const clang::QualType& qualType);
   
   
   void traverseStructureField( Json::Value& value, const clang::QualType& qualType, const char* fieldName = "\0" );
   
   
   
   const clang::FunctionDecl* _funcDecl;
   
//    std::string _name;


   std::map< const std::string, const clang::DeclaratorDecl*> _mockFunctions;

   
   typedef std::pair<const clang::DeclaratorDecl*, std::string> DeclValuePair;
   typedef std::map< std::string, DeclValuePair > NameDeclValueMap;
   
   typedef std::pair<clang::QualType, std::string> QualTypeValuePair;
   typedef std::map< std::string, QualTypeValuePair > NameQualTypeValueMap;
   
   NameQualTypeValueMap _input;
   NameQualTypeValueMap _output;
   NameDeclValueMap _mocks;
   
   
   //    friend std::ostream& operator << (std::ostream& os, const FuncParamsStruct& obj);
   
};


// std::ostream& operator << (std::ostream& os, const FuncParamsStruct& obj);



#endif // _UTBuilder_FuncParamsStructure_h__

