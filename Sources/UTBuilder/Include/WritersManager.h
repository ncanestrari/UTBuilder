#ifndef _UTBuilder_WritersManager_h__
#define _UTBuilder_WritersManager_h__


#include <string>
#include <set>

#include "UnitTestData.h"

#include <clang/Basic/SourceManager.h>


class BaseWriter;


class WritersManager {
   
public:
   
   WritersManager() = delete;
   
   WritersManager(const std::string&         fileName,
	       const UnitTestData&           data,
	       const clang::SourceManager&   sourceMgr);
   
   ~WritersManager();
   
   void add(BaseWriter* writer);
   
   void write();
   
private:
   
   const std::string            _fileName;
   const UnitTestData&          _data;
   const clang::SourceManager&	_sourceMgr;
   
   std::set< std::unique_ptr<BaseWriter> > _writers;
   
};



#endif // _UTBuilder_WritersManager_h__

