#ifndef _UTBuilder_ProjectDescription_h__
#define _UTBuilder_ProjectDescription_h__

#include <boost/filesystem.hpp>

#include <set>
#include <string>
#include <vector>
#include <json/json.h>

#include "optionparser.h"


class ProjectDescription 
{
public:
   ProjectDescription(){}
   ~ProjectDescription(){}
   //ProjectDescription(const ProjectDescription&);
   ProjectDescription& operator=(ProjectDescription const&);
   
   //    Serializable interface
   void serializeJson(Json::Value &jsonRoot ) const;
   void deSerializeJson(const Json::Value &jsonRoot, const void *refData = nullptr);
   
   void clear(void);
   void init(const OptionParser& optionParser );
   void createFakeSource(void); // might go to utils...
   void getFromOptionParser(const OptionParser& optionParser);
   
   //getters 
   const std::vector<std::string>&  getAllFileNames(void){ return _fileNames; }
   const std::vector<std::string>&  getAllFileNames(void) const { return _fileNames; }
   
   const std::string& getInputFileName(void){ return _inputFileName; }
   const std::string& getInputFileName(void) const { return _inputFileName; }
   
   //const std::vector<std::string>& getIncludePaths(void){ return _includePaths; }
   //const std::vector<std::string>& getIncludePaths(void) const { return _includePaths; }
   
   const std::string & getOutputFileName(void) { return _outputFileName; }
   const std::string & getOutputFileName(void) const { return _outputFileName; }
   
   const std::set<boost::filesystem::path> & getSources(void) { return _sources; }
   const std::set<boost::filesystem::path> & getSources(void) const { return _sources; }
   
   const std::set<boost::filesystem::path> & getPackages(void) { return _packages; }
   const std::set<boost::filesystem::path> & getPackages(void) const { return _packages; }
   
   const std::set<boost::filesystem::path> & getIncludePaths(void) { return _include_dirs; }
   const std::set<boost::filesystem::path> & getIncludePaths(void) const { return _include_dirs; }
   
   const std::set<boost::filesystem::path> & getSourcesPaths(void) { return _sources_dirs; }
   const std::set<boost::filesystem::path> & getSourcesPaths(void) const { return _sources_dirs; }
   
   
   
protected:   
   
   // read from json "desc" object
   std::vector<std::string> _fileNames;
   std::vector<std::string> _dirNames;
   std::vector<std::string> _includePaths;
   
   std::string _inputFileName;
   std::string _outputFileName;
   std::string _workspace;
   
   std::set<boost::filesystem::path> _sources;
   std::set<boost::filesystem::path> _packages;
   std::set<boost::filesystem::path> _include_dirs;
   std::set<boost::filesystem::path> _sources_dirs;
   
   const OptionParser* _optionParser;
   
};

#endif /* _UTBuilder_ProjectDescription_h__ */
