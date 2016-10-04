

#include "ProjectDescription.h"

#include <string>
#include <vector>
#include <json/json.h>

#include "globber.h"

using std::string;
using std::vector;
using Json::Value;

void FillWithArrayOfStringFromField(vector<string> &vstr, const Value &rootValue, const char *key)
{
   size_t size = rootValue[key].size();
   vstr.resize(size);
   for( int i = 0; i < size; ++i){
      vstr.push_back( rootValue[key][i].getAsString() );
   }   
}

void ProjectDescription::deserializeJson(const Value &rootDesc)
{
   FillWithArrayOfStringFromField(_fileNames, rootDesc, "files");
   FillWithArrayOfStringFromField(_dirNames,  rootDesc, "dirs");
   _outputFileName = rootDesc["output"].getAsString();
   for( const auto &dir : _dirNames ){
      //get all c files in dir
      vector<string> cfiles = glob(dir+"*.c");//TODO full path?
      _fileNames.insert(_fileNames.begin(), cfiles.begin(), cfiles.end());
      cfiles = = glob(dir+"*.cpp");//TODO full path?
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
