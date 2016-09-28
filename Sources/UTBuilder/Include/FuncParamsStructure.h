
#ifndef _UTBuilder_FuncParamsStructure_h__
#define _UTBuilder_FuncParamsStructure_h__

#include <clang/AST/Decl.h>
#include <iostream>
#include <json/json.h>
#include <map>
#include <set>
#include <string>

#include "NameValueTypeNode.h"


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

   void serialize(Json::Value &jsonRoot, const bool withMocks = true);

   void deSerialize(const Json::Value &jsonRoot);

   void deSerializeJson(const FuncParamsStruct &funcParam, const Json::Value &jsonRoot);

   void writeAsStruct(void);

private:

   void clear(void);

   std::shared_ptr<NameValueTypeNode<clang::QualType> > buildInputTree(const clang::FunctionDecl *funcDecl);

   std::shared_ptr<NameValueTypeNode<clang::QualType> > buildOutputTree(const clang::FunctionDecl *funcDecl);

   std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > buildMockFuncsTree(const std::set<const clang::FunctionDecl *> &mockFuncs);


   static void serializeTree(std::shared_ptr<NameValueTypeNode<clang::QualType> > tree, Json::Value &jsonRoot);

   static void serializeTree(std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > tree, Json::Value &jsonRoot);


   static void deSerializeTree(std::shared_ptr<NameValueTypeNode<clang::QualType> > tree, const Json::Value &jsonRoot);

   static void deSerializeTree(std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > tree, const Json::Value &jsonRoot);



   static std::shared_ptr<NameValueTypeNode<clang::QualType> >
   deSerializeTreeJson(const std::shared_ptr<NameValueTypeNode<clang::QualType> > referenceTree,
                       const Json::Value &fieldItem);


   static std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> >
   deSerializeTreeJson(const std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > tree, const Json::Value &fieldItem);


   const clang::FunctionDecl *_funcDecl;


   // vectors of trees
   std::vector< std::shared_ptr<NameValueTypeNode<clang::QualType> > > _inputTree;
   std::vector< std::shared_ptr<NameValueTypeNode<clang::QualType> > > _outputTree;
   std::vector< std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > > _mocksTree;

public:

   // temp functions to  generate files automatically
   static void  writeAsStructure(std::ostringstream &os, const FuncParamsStruct &obj);

   static void  writeGoogleTest(std::ostringstream &os, const FuncParamsStruct &obj,  const unsigned int i);

   const std::vector< std::shared_ptr<NameValueTypeNode<clang::QualType> > > &getInputTree(void) const
   {
      return  _inputTree;
   }
   const std::vector< std::shared_ptr<NameValueTypeNode<clang::QualType> > > &getOutputTree(void) const
   {
      return _outputTree;
   }
   const std::vector< std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > > &getMockTree(void) const
   {
      return _mocksTree;
   }
};


#endif // _UTBuilder_FuncParamsStructure_h__

