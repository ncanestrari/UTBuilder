
#ifndef _UTBuilder_FunctionTestDataFile_h__
#define _UTBuilder_FunctionTestDataFile_h__

#include "FunctionTestCollection.h"

#include <clang/Frontend/CompilerInstance.h>
#include <json/json.h>

#include "ProjectDescription.h"
#include "singleton.h"



class FunctionTestDataFile :  public Serializable, public Singleton<FunctionTestDataFile> {
public:

   void initCollections(const FunctionDeclKeySetMap   &funcDeclsMap,
                        const FunctionDeclKeySetMap   &mockDeclsMap);
   
   //    Serializable interface
   virtual void serializeJson(Json::Value &jsonRoot ) const override;
   virtual void deSerializeJson(const Json::Value &jsonRoot, const void *refData = nullptr) override;
      
   //getters
   const clang::CompilerInstance & getCompilerInstance(void){ return _compiler; }
   const clang::CompilerInstance & getCompilerInstance(void) const { return _compiler; }
   const FunctionTestCollection * getUnitTestCollection(void){ return _unitFunctionTestCollection.get(); }
   const FunctionTestCollection * getUnitTestCollection(void) const { return _unitFunctionTestCollection.get(); }
   const FunctionTestCollection * getMockTestCollection(void){ return _mockFunctionTestCollection.get(); }
   const FunctionTestCollection * getMockTestCollection(void) const { return _mockFunctionTestCollection.get(); }
   const ProjectDescription & getProjectDescription(void){ return _projectDescription; }
   const ProjectDescription & getProjectDescription(void) const { return _projectDescription; }
protected:
   void computeAST(void) const;
   void clearCollections(void);
    
   mutable clang::CompilerInstance _compiler;
//    UnitFunctionTestCollection _unitFunctionTestCollection;
//    MockFunctionTestCollection _mockFunctionTestCollection;
   std::shared_ptr<FunctionTestCollection> _unitFunctionTestCollection;
   std::shared_ptr<FunctionTestCollection> _mockFunctionTestCollection;
   mutable ProjectDescription _projectDescription;
};


#endif /* _UTBuilder_FunctionTestDataFile_h__ */

