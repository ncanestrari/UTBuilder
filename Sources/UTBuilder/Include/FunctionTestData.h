
#ifndef _UTBuilder_FunctionTestData_h__
#define _UTBuilder_FunctionTestData_h__

// #include <clang/AST/Decl.h>
#include <iostream>
#include <json/json.h>

#include <string>
#include <set>

#include "NameValueTypeNode.h"

namespace clang{
   class FunctionDecl;
}


class FunctionTestData {

public:
//    FunctionTestData( const std::String& name, const clang::FunctionDecl *funcDecl,
//              const std::set<const clang::FunctionDecl *> &mockFuncs = std::set<const clang::FunctionDecl *>());
   
   FunctionTestData( const clang::FunctionDecl *funcDecl,
             const std::set<const clang::FunctionDecl *> &mockFuncs = std::set<const clang::FunctionDecl *>());
   
   FunctionTestData() = delete;
   FunctionTestData(const FunctionTestData&) = delete;
   
   ~FunctionTestData();



   void serializeJson(Json::Value &jsonRoot );


   void deSerializeJson(const FunctionTestData *funcTestData, const Json::Value &jsonRoot);

   void writeAsStruct(void);

//    const std::string& getName(void) const
//    {
//       return _name;
//    }
   
   const std::shared_ptr<NameValueTypeNode<clang::QualType> > &getInputTree(void) const
   {
      return  _inputTree;
   }
   
   const std::shared_ptr<NameValueTypeNode<clang::QualType> > &getOutputTree(void) const
   {
      return _outputTree;
   }
   
   const std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > &getMockTree(void) const
   {
      return _mocksTree;
   }
      
      // temp functions to  generate files automatically
   static void  writeAsStructure(std::ostringstream &os, const clang::FunctionDecl *funcDecl);

   
   void  writeGoogleTest(std::ostringstream &os, const clang::FunctionDecl *funcDecl);
   
   
   
private:

   void clear(void);
   

   std::shared_ptr<NameValueTypeNode<clang::QualType> > buildInputTree(const clang::FunctionDecl *funcDecl);

   std::shared_ptr<NameValueTypeNode<clang::QualType> > buildOutputTree(const clang::FunctionDecl *funcDecl);

   std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > buildMockFuncsTree(const std::set<const clang::FunctionDecl *> &mockFuncs);


   static void serializeJsonTree(std::shared_ptr<NameValueTypeNode<clang::QualType> > tree, Json::Value &jsonRoot);

   static void serializeJsonTree(std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > tree, Json::Value &jsonRoot);


   static std::shared_ptr<NameValueTypeNode<clang::QualType> >
   deSerializeTreeJson(const std::shared_ptr<NameValueTypeNode<clang::QualType> > referenceTree,
                       const Json::Value &fieldItem);


   static std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> >
   deSerializeTreeJson(const std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > tree, const Json::Value &fieldItem);



//    std::string _functionName;
   
   // vectors of trees
   std::shared_ptr<NameValueTypeNode<clang::QualType> > _inputTree;
   std::shared_ptr<NameValueTypeNode<clang::QualType> >  _outputTree;
   std::shared_ptr<NameValueTypeNode<const clang::FunctionDecl *> > _mocksTree;



   // temp functions to  generate files automatically
//    static void  writeAsStructure(std::ostringstream &os, const FuncParamsStruct &obj);

//    static void  writeGoogleTest(std::ostringstream &os, const FuncParamsStruct &obj,  const unsigned int i);

};


#endif // _UTBuilder_FunctionTestData_h__

