
#ifndef _UTBuilder_FunctionTestDataFile_h__
#define _UTBuilder_FunctionTestDataFile_h__

#include "FunctionTestCollection.h"

#include <clang/Frontend/CompilerInstance.h>
#include <json/json.h>

#include "ProjectDescription.h"
#include "singleton.h"



class FunctionTestDataFile :  public Singleton<FunctionTestDataFile> {
public:

   void deSerializeJson(const Json::Value &jsonRoot);   
   void serializeJson(Json::Value &jsonRoot);
   
   //getters
   const clang::CompilerInstance & getCompilerInstance(void){ return _compiler; }
   const clang::CompilerInstance & getCompilerInstance(void) const { return _compiler; }
   const UnitFunctionTestCollection & getUnitTestCollection(void){ return _unitFunctionTestCollection; }
   const UnitFunctionTestCollection & getUnitTestCollection(void) const { return _unitFunctionTestCollection; }
   const MockFunctionTestCollection & getMockTestCollection(void){ return _mockFunctionTestCollection; }
   const MockFunctionTestCollection & getMockTestCollection(void) const { return _mockFunctionTestCollection; }
   const ProjectDescription & getProjectDescription(void){ return _projectDescription; }
   const ProjectDescription & getProjectDescription(void) const { return _projectDescription; }
protected:
   void computeAST(void);
   clang::CompilerInstance _compiler;
   UnitFunctionTestCollection _unitFunctionTestCollection;
   MockFunctionTestCollection _mockFunctionTestCollection;
   ProjectDescription _projectDescription;
};


#endif /* _UTBuilder_FunctionTestDataFile_h__ */

