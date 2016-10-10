#ifndef _UTBuilder_Serializable_h__
#define _UTBuilder_Serializable_h__


#include <json/json.h>


class Serializable 
{
public:

   Serializable() {}
   ~Serializable() {}
   
   virtual void serializeJson(Json::Value &jsonRoot) const = 0;
   
   virtual void deSerializeJson(const Json::Value &jsonRoot, const void* refData) = 0;

};



#endif // _UTBuilder_Serializable_h__
 
