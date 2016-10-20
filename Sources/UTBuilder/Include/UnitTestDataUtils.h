#ifndef _UTBuilder_UnitTestDataUtils_h__
#define _UTBuilder_UnitTestDataUtils_h__


#include <json/json.h>


class NameValueNode;


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

};


#endif /*_UTBuilder_UnitTestDataUtils_h__*/