
#include "WritersManager.h"

#include "Writers.h"

#include <boost/filesystem.hpp>
using boost::filesystem::create_directory;
using boost::filesystem::current_path;
using boost::filesystem::exists;
using boost::filesystem::is_directory;
using boost::filesystem::path;


WritersManager::WritersManager(const std::string&     fileName,
                               const UnitTestData&    data,
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
   path tmp = path("Tests");
   create_directory(tmp);
   tmp /= path(_fileName+".dir");
   create_directory(tmp);
   
   path ci = current_path();
   current_path(tmp);
   
   for (const auto& writer : _writers ){
      writer->writeTemplate( writer->getTemplateName(), writer->getTemplateSuffix() );
   }
   
   current_path(ci);
}

