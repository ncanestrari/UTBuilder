#ifndef _UTBuilder_UnitTestDataUtils_h__
#define _UTBuilder_UnitTestDataUtils_h__


#include <json/json.h>


class NameValueNode;
// template<typename T> class TypeNameValueNode;
class QualTypeNode;
class FunctionDeclNode;

namespace clang {
   class FunctionDecl;
   class QualType;
}

class UnitTestDataUtils 
{
public:
   
   static NameValueNode* findContentFromAST(const std::string &key, const NameValueNode* dataAST);
   
//    comment in json example file
   static void addJsonArrayComment(Json::Value& jsonValue, const NameValueNode* node);
   static void addJsonArrayElementComment(Json::Value& jsonValue, const NameValueNode* node);
   static void addJsonObjectComment(Json::Value& jsonValue, const NameValueNode* node);
   static void addJsonValueComment(Json::Value& jsonValue, const NameValueNode* node);
   static void addJsonStructValueComment(Json::Value& jsonValue, const NameValueNode* node);

   static void writeFunctionDeclAsStructure(std::ostringstream &os, const clang::FunctionDecl* funcDecl );
   static void writeGoogleTest(std::ostringstream &os,  const clang::FunctionDecl* funcDecl, const NameValueNode* contentElement );
   
   
private:
   
   static void writeStructureValue( std::ostringstream &os,
                                    const NameValueNode* tree,
                                    const std::string &name,
                                    const std::string &indent);
   
   static void writeStructureComparison( std::ostringstream &os,
                                         const NameValueNode* tree,
                                         const std::string &name,
                                         const std::string &indent );
   
};


#endif /*_UTBuilder_UnitTestDataUtils_h__*/