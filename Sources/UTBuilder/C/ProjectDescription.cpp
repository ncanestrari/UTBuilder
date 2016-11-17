

#include "ProjectDescription.h"

#include <boost/filesystem/convenience.hpp> 
using boost::filesystem::current_path;
using boost::filesystem::exists;
using boost::filesystem::path;


#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>
using std::endl;
using std::logic_error;
using std::ofstream;
using std::ifstream;
using std::set;
using std::string;
using std::vector;

#include <json/json.h>
using Json::Value;

#include "globber.h"
#include "optionparser.h"
#include "Utils.h"

const std::string ProjectDescription::_inputFileName = "fakesource.c";

void FillWithArrayOfStringFromField(vector<string> &vstr, const Value &rootValue, const char *key)
{
   size_t size = rootValue[key].size();
   for( int i = 0; i < size; ++i){
      vstr.push_back( rootValue[key][i].asString() );
   }   
}



void ProjectDescription::serializeJson(Json::Value &jsonRoot ) const
{
   if ( ! _optionParser->isJsonFileNameEnabled() ) {
      jsonRoot["output"] =  _optionParser->getOutputName();
      jsonRoot["files"] = Json::Value(Json::arrayValue);
      
      vector<string> fileNames;
      _optionParser->getFileNames(fileNames);
      for (unsigned int i = 0; i < fileNames.size(); ++i) {
         jsonRoot["files"][i] = fileNames[i];
      }
   }
}


void ProjectDescription::deSerializeJson(const Value &rootDesc, const void * )
{
   /**
    *  "desc" 
    *      {
    *      "files" : {}   array
    *      "dirs" : {}  array
    *      "output" : []  value
    *       }
    */
   
   FillWithArrayOfStringFromField(_fileNames, rootDesc, "files");
   FillWithArrayOfStringFromField(_dirNames,  rootDesc, "dirs");
   _outputFileName = rootDesc["output"].asString();
   set<string> cfiles;
   for( const auto &dir : _dirNames ){
      //get all c files in dir
      glob(dir+"*.c", cfiles);
      glob(dir+"*.cpp", cfiles);
   }
   _fileNames.insert(_fileNames.end(), cfiles.begin(), cfiles.end());
}


void ProjectDescription::clear(void)
{
   _outputFileName.clear();
   _fileNames.clear();
   _dirNames.clear();
   _sources.clear();
   _packages.clear();
   _include_dirs.clear();
   _sources_dirs.clear();
}


void ProjectDescription::init(const OptionParser& optionParser)
{
   
   _optionParser = &optionParser;
   
   getFromOptionParser(optionParser);
   
   /**
    * Clever HACK
    * include the source file directories to allow multiple files:
    * clang ast computes only a file at a time, so by creating a file as
    *   #include "file_01.c"
    *   #include "file_02.c"
    *   ...
    * and use this as a source, we can compute the ast for multiple files
    * AND preserve the file origin and position of each declarations.
    * 
    */
   
   _workspace = current_path();
   
   for (const string & fname : _fileNames){
      path p = path(fname);
      if( ! exists(p) ){
         throw logic_error("source file\n\n\t" + fname + "\ndoes not exists\n");
      }
      _sources.insert(p);
   }

   for (const path &source : _sources){
      _packages.insert(  *( source.begin() )  );
   }

   Utils::getIncludePaths(_packages, _include_dirs);
   Utils::getSourcesPaths(_packages, _sources_dirs);
   
}


void ProjectDescription::createFakeSource(void)
{
   ofstream inputFile;
   inputFile.open(_inputFileName);
   if( ! inputFile.is_open() ){
      throw logic_error("cannot create fakesource.c file\n");
   }
   for(const path &source : _sources){
      inputFile << "#include " << source.filename() << endl;// path override the << operator, but writes using quotes.
   }
   inputFile.close();
}


void ProjectDescription::getFromOptionParser(const OptionParser& optionParser)
{
   
   if ( optionParser.isJsonFileNameEnabled() ) {
      // we need to fil _fileNames form the input args or from the json file "desc" : {"files"}
      const string jsonFileName = optionParser.getJsonFileName();
      ifstream jsonFile;
      jsonFile.open(jsonFileName);
      
      if (jsonFile.is_open() == false) {
         throw logic_error("json file\n\n\t" + jsonFileName + "\n\nnot found\n");
      }
      
      Json::Value jsonRoot;
      jsonFile >> jsonRoot;   
      deSerializeJson(jsonRoot["desc"]);     
      jsonFile.close();
      
   } else {
       //add from command line instead of desc
      optionParser.getFileNames(_fileNames);
      _outputFileName = optionParser.getOutputName();
   }

}

