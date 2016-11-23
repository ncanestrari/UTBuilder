
#include "WritersManager.h"

#include <plustache_types.hpp>
#include <template.hpp>


#include <boost/filesystem.hpp>
using boost::filesystem::create_directory;
using boost::filesystem::current_path;
using boost::filesystem::exists;
using boost::filesystem::is_directory;
using boost::filesystem::path;
using std::cout;
using std::string;
using std::ifstream;
using std::stringstream;
using PlustacheTypes::ObjectType;
using PlustacheTypes::CollectionType;
using Plustache::Context;
using Plustache::template_t;


const std::string BaseWriter::_templateDir =  std::getenv("TEMPLATE_DIR");


void BaseWriter::init(const std::string&    fileName,
                      const UnitTestData*   data,
                      const ClangCompiler*  compiler )
{
   _fileName = fileName;
   _data = data;
   _info = &compiler->getASTinfo();
   _sourceMgr = &compiler->getSourceManager();
}


void BaseWriter::writeTemplate(const std::string& templateFileName, const std::string& outputNameSuffix)
{
   
   auto context = std::unique_ptr<const Plustache::Context>( createContext() );
   
   if ( context == nullptr ) {
      std::cout << "WARNING: BaseWriter::WriteTemplate() _context has not been created" << std::endl;
      return;
   }
   
   template_t     t;
   stringstream   buffer;
   string         result;

   boost::filesystem::path full_path(boost::filesystem::current_path());
   ifstream template_file( templateFileName );

   if (template_file.fail()) {
      std::cout << "template file for mock functions not found";
      return;
   }

   buffer << template_file.rdbuf();
   string         template_buff(buffer.str());

   
   result = t.render(template_buff, *context);

   std::ofstream outputFile;
   std::string outputFileName = _fileName + outputNameSuffix;
   outputFile.open(outputFileName, std::fstream::out);
   if ( outputFile.is_open() ){
     outputFile << result;
     std::cout << "file written: " << outputFileName << std::endl;
   }
   else {
     std::cout << "ERROR: opening path to write: " << outputFileName << " doesn't exist" << std::endl;
   }
   outputFile.close();

   
}



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

