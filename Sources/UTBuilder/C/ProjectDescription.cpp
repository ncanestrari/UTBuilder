

#include "ProjectDescription.h"

#include <boost/filesystem/convenience.hpp> 
#include <fstream>
#include <string>
#include <vector>
#include <json/json.h>

#include "globber.h"
#include "optionparser.h"
#include "utils.h"

using std::endl;
using std::ofstream;
using std::string;
using std::vector;
using Json::Value;

void FillWithArrayOfStringFromField(vector<string> &vstr, const Value &rootValue, const char *key)
{
   size_t size = rootValue[key].size();
   vstr.resize(size);
   for( int i = 0; i < size; ++i){
      vstr.push_back( rootValue[key][i].asString() );
   }   
}

void ProjectDescription::deserializeJson(const Value &rootDesc)
{
   FillWithArrayOfStringFromField(_fileNames, rootDesc, "files");
   FillWithArrayOfStringFromField(_dirNames,  rootDesc, "dirs");
   _outputFileName = rootDesc["output"].asString();
   for( const auto &dir : _dirNames ){
      //get all c files in dir
      vector<string> cfiles = glob(dir+"*.c");//TODO full path?
      _fileNames.insert(_fileNames.begin(), cfiles.begin(), cfiles.end());
      cfiles = glob(dir+"*.cpp");//TODO full path?
      _fileNames.insert(_fileNames.begin(), cfiles.begin(), cfiles.end());
   }
}

void ProjectDescription::setOutputFileName(const string &outputFileName)
{
   _outputFileName = outputFileName;
}

void ProjectDescription::setFileNames(const vector<string> &fileNames)
{
   _fileNames = fileNames;
}

void ProjectDescription::setDirNames(const vector<string> &dirNames)
{
   _dirNames = dirNames;
}

vector<string>& ProjectDescription::getAllFileNames(void)
{
   return _fileNames;
}

void ProjectDescription::clear(void)
{
   _outputFileName = "";
   _fileNames = {};
   _dirNames = {};
}

void ProjectDescription::init(void)
{
   //set _workspace
   string ci;
   OptionParser::get().getCommercialCode(ci);
   string package;
   OptionParser::get().getPackage(package);
   _workspace = ci + "/" + package + "/";
   //fill IncludePaths
   utils::getIncludePaths( _workspace, _includePaths);
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
   utils::getSourcePaths( _workspace, _includePaths);
   _inputFileName = _workspace + "fakesource.c";
}

void ProjectDescription::createFakeSource(void)
{
   ofstream inputFile;
   inputFile.open(_inputFileName);
   for( const auto &fileName : _fileNames){
      boost::filesystem::path filepath(fileName);
      inputFile << "#include \"" << basename(filepath) << "\"" << endl;
   }
   inputFile.close();
}

string& ProjectDescription::getPackageFullPath(void)
{
   return _workspace;
}

string& ProjectDescription::getInputFileName(void)
{
   return _inputFileName;
}

vector<string>& ProjectDescription::getIncludePaths(void)
{
   return _includePaths;
}