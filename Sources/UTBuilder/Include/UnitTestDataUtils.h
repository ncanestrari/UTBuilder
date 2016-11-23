#ifndef _UTBuilder_UnitTestDataUtils_h__
#define _UTBuilder_UnitTestDataUtils_h__


#include <json/json.h>


class NameValueNode;
class QualTypeNode;
class FunctionDeclNode;

namespace clang {
   class FunctionDecl;
   class QualType;
   class SourceManager;
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
   static void writeGlobalsContent(std::ostringstream &os, const std::string& name, const NameValueNode* contentElement);
  
   static void writeFunctionDefinition( const std::string&           name,
				        const clang::FunctionDecl*   funcDecl,
				        const NameValueNode*	     outTree,
				        std::ostringstream&          out);
   
   
   static void writeMockFunctionFFF(const clang::FunctionDecl*  funcDecl,
				    const clang::SourceManager* _sourceMgr,
				    std::ostringstream&	out);
   
private:
   
   static void writeStructureValue( std::ostringstream &os,
                                    const NameValueNode* tree,
                                    const std::string &name,
                                    const std::string &indent);
   
   static void writeStructureComparison( std::ostringstream &os,
                                         const NameValueNode* tree,
                                         const std::string &name,
                                         const std::string &indent );
   
   static void writeMockValue(	std::ostringstream &os,
				const NameValueNode*  tree,
				const std::string &name);
   
   
};


#endif /*_UTBuilder_UnitTestDataUtils_h__*/