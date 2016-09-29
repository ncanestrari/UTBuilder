
#ifndef _UTBuilder_DataFile_h__
#define _UTBuilder_DataFile_h__


#include "FunctionTestCollection.h"

// for the singleton template
#include "Results.h"




class DataFile :  public Singleton<DataFile> {
   
public:
   
   void clearCollections();
   
   void initCollections(const FunctionDeclKeySetMap   &funcDeclsMap);
   
   void deSerializeJson(const Json::Value &jsonRoot);
   
   void serializeAST(Json::Value &jsonRoot) const;
   
   UnitFunctionTestCollection unitFunctionTestCollection;
   
   MockFunctionTestCollection mockFunctionTestCollection;
};




#endif // _UTBuilder_DataFile_h__

