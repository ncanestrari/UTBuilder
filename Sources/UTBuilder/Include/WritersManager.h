#ifndef _UTBuilder_WritersManager_h__
#define _UTBuilder_WritersManager_h__


#include <string>
#include <set>

#include "UnitTestData.h"

#include "ClangCompiler.h"


class BaseWriter;


class WritersManager {
   
public:
   
   WritersManager() = delete;
   
   WritersManager(const std::string&         fileName,
	       const UnitTestData&           data,
	       const ClangCompiler& compiler);
   
   ~WritersManager();
   
   void add(BaseWriter* writer);
   
   void write();
   
private:
   
//    keep a unique_ptr ref of the Writers   
   std::set< std::unique_ptr<BaseWriter> > _writers;
      
      
   // store all these vars and pass them to each Writers as argument when the Writers are added to the WritersManager
   const std::string            _fileName;
   const UnitTestData*          _data;
   const ClangCompiler*		_compiler;
//    const clang::SourceManager&	_sourceMgr;
   
};



#endif // _UTBuilder_WritersManager_h__

