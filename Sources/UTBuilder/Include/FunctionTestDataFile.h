
#ifndef _UTBuilder_FunctionTestDataFile_h__
#define _UTBuilder_FunctionTestDataFile_h__

#include "Serializable.h"
#include "FunctionTestCollection.h"

// for the singleton template
#include "Results.h"




class FunctionTestDataFile :  public Serializable , public Singleton<FunctionTestDataFile> {
   
public:
   
   FunctionTestDataFile();
   
   ~FunctionTestDataFile();
   
   
   void initCollections(const FunctionDeclKeySetMap   &funcDeclsMap,
                        const FunctionDeclKeySetMap   &mockDeclsMap);
   

   //    Serializable interface
   virtual void serializeJson(Json::Value &jsonRoot ) const override;

   virtual void deSerializeJson(const Json::Value &jsonRoot, const void *refData = nullptr) override;
   
   
   
//    UnitFunctionTestCollection unitFunctionTestCollection;
//    MockFunctionTestCollection mockFunctionTestCollection;
   
   std::shared_ptr<FunctionTestCollection> _unitFunctionTestCollection;
   std::shared_ptr<FunctionTestCollection> _mockFunctionTestCollection;
   
   
private:
   
   void clearCollections();
   
};


#endif /* _UTBuilder_FunctionTestDataFile_h__ */
