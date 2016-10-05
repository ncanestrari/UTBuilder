
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
   
   UnitFunctionTestCollection unitFunctionTestCollection;
   MockFunctionTestCollection mockFunctionTestCollection;
   ProjectDescription projectDescription;
   clang::CompilerInstance compiler;
protected:
   void computeAST(void);
   
};


#endif /* _UTBuilder_FunctionTestDataFile_h__ */
