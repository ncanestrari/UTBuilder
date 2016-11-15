#include "WritersManager.h"

#include "Writers.h"


WritersManager::WritersManager(const std::string&         fileName,
	    const UnitTestData&           data,
	    const ClangCompiler&   compiler)
: _fileName(fileName)
, _data(&data)
, _compiler(&compiler)
{
}


WritersManager::~WritersManager() = default;


void WritersManager::add(BaseWriter* writer) 
{
   if ( writer == nullptr )
      return;
   
   writer->init(_fileName, _data, _compiler );
   _writers.insert( std::unique_ptr<BaseWriter>(writer) );
}


void WritersManager::write()
{
   for (const auto& writer : _writers )
   {
      writer->writeTemplate( writer->getTeamplateName(), writer->getTeamplateSuffix() );
   }
}
   
   
 

