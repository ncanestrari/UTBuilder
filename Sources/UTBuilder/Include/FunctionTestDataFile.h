
#ifndef _UTBuilder_FunctionTestDataFile_h__
#define _UTBuilder_FunctionTestDataFile_h__

#include "FunctionTestCollection.h"

// for the singleton template
#include "Results.h"




class FunctionTestDataFile :  public Singleton<FunctionTestDataFile> {
   
protected:
   
//    DataFile() {}
   
public:
   
   void clearCollections();
   
   void initCollections(const FunctionDeclKeySetMap   &funcDeclsMap,
                        const FunctionDeclKeySetMap   &mockDeclsMap);
   
   void deSerializeJson(const Json::Value &jsonRoot);
   
   void serializeJson(Json::Value &jsonRoot) const;
   
   UnitFunctionTestCollection unitFunctionTestCollection;
   
   MockFunctionTestCollection mockFunctionTestCollection;
};


#endif /* _UTBuilder_FunctionTestDataFile_h__ */
