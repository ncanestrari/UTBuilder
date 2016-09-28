
#ifndef _UTBuilder_FuncParamsStructure_h__
#define _UTBuilder_FuncParamsStructure_h__

#include <clang/AST/Decl.h>
#include <iostream>
#include <json/json.h>
#include <map>
#include <set>
#include <string>

// #include "NameValueTypeNode.h"

#include "FunctionTestData.h"


class FuncParamsStruct {

public:
   FuncParamsStruct();
   ~FuncParamsStruct();

   void init(const clang::FunctionDecl *funcDecl,
             const std::set<const clang::FunctionDecl *> &mockFuncs = std::set<const clang::FunctionDecl *>());


   std::string getName(void) const
   {
      return _funcDecl->getNameAsString();
   }

   std::string getName(const unsigned int index) const;

   size_t getNumParams(void) const
   {
      return _funcDecl->getNumParams();
   }

   const clang::FunctionDecl *getFunctionDecl() const
   {
      return _funcDecl;
   }

   unsigned int getSize(void) const;

   
   void serializeJson(Json::Value &jsonRoot );

   void deSerializeJson(const FuncParamsStruct &funcParam, const Json::Value &jsonRoot);

   
   void writeAsStruct(void);


      
   // temp functions to  generate files automatically
   static void  writeAsStructure(std::ostringstream &os, const FuncParamsStruct &obj);

   static void  writeGoogleTest(std::ostringstream &os, const FuncParamsStruct &obj,  const unsigned int i);
   
   
   
private:

   void clear(void);


   static void serializeJsonTree(std::shared_ptr<NameValueTypeNode<clang::QualType> > tree, Json::Value &jsonRoot);

   static void serializeJsonTree(std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > tree, Json::Value &jsonRoot);


   static std::shared_ptr<NameValueTypeNode<clang::QualType> >
   deSerializeTreeJson(const std::shared_ptr<NameValueTypeNode<clang::QualType> > referenceTree,
                       const Json::Value &fieldItem);


   static std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> >
   deSerializeTreeJson(const std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > tree, const Json::Value &fieldItem);


   const clang::FunctionDecl *_funcDecl;


   std::vector< std::shared_ptr<FunctionTestData> > _tests;

};


#endif // _UTBuilder_FuncParamsStructure_h__

