#ifndef _UTBuilder_FuncParamsStructure_h__
#define _UTBuilder_FuncParamsStructure_h__

// #include <clang/AST/Type.h>
#include <clang/AST/Decl.h>

#include <string>
#include <set>
#include <map>
#include <iostream>

#include <json/json.h>

#include "NameValueTypeNode.h"

// namesp/ace clang {
//   class QualType;
//   class FunctionDecl;
// }


class FuncParamsStruct
{

public:
    FuncParamsStruct();
    ~FuncParamsStruct();
    
    void init( const clang::FunctionDecl* funcDecl, 
               const std::set<const clang::FunctionDecl*>& mockFuncs = std::set<const clang::FunctionDecl*>() );   
    
    
    const char* getName(void) const { return _funcDecl->getNameAsString().c_str(); }    
    
    size_t getNumParams(void) const { return _funcDecl->getNumParams(); }
    
    
    void serializeJson(Json::Value& jsonRoot, const bool withMocks = true);
    
    void deSerializeJson(const Json::Value& jsonRoot);
    
    void writeAsStruct(void);
    
private:
   
   void clear(void);
   
   std::shared_ptr<NameValueTypeNode<clang::QualType> > buildInputTree( const clang::FunctionDecl* funcDecl);
   
   std::shared_ptr<NameValueTypeNode<clang::QualType> > buildOutputTree( const clang::FunctionDecl* funcDecl);
   
   std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl*> > buildMockFuncsTree( const std::set<const clang::FunctionDecl*>& mockFuncs );
   
   
   static void serializeTree( std::shared_ptr<NameValueTypeNode<clang::QualType> > tree, Json::Value& jsonRoot);
   
   static void serializeTree( std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl*> > tree, Json::Value& jsonRoot);
   
   
   static void deSerializeTree( std::shared_ptr<NameValueTypeNode<clang::QualType> > tree, const Json::Value& jsonRoot);
   
   static void deSerializeTree( std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl*> > tree, const Json::Value& jsonRoot);
   

   
   const clang::FunctionDecl* _funcDecl;
   

   // vectors of trees 
   std::vector< std::shared_ptr<NameValueTypeNode<clang::QualType> > > _inputTree;
   std::vector< std::shared_ptr<NameValueTypeNode<clang::QualType> > > _outputTree;
   std::vector< std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl*> > > _mocksTree;
 
   
};



#endif // _UTBuilder_FuncParamsStructure_h__

