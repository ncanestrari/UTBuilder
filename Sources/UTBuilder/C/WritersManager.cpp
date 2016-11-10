#include "WritersManager.h"

#include "Writers.h"


WritersManager::WritersManager(const std::string&         fileName,
	    const UnitTestData&           data,
	    const clang::SourceManager&   sourceMgr)
: _fileName(fileName)
, _data(data)
, _sourceMgr(sourceMgr)
{
}


WritersManager::~WritersManager() = default;


void WritersManager::add(BaseWriter* writer) 
{
   if ( writer == nullptr )
      return;
   
   writer->init(_fileName, _data, _sourceMgr);
   _writers.insert( std::unique_ptr<BaseWriter>(writer) );
}


void WritersManager::write()
{
   for (const auto& writer : _writers )
   {
      writer->writeTemplate( writer->getTeamplateName(), writer->getTeamplateSuffix() );
   }
}
   
   
 

