#ifndef _UTBuilder_ProjectDescription_h__
#define _UTBuilder_ProjectDescription_h__

#include <string>
#include <vector>
#include <json/json.h>


class ProjectDescription{
public:
   ProjectDescription(){}
   ~ProjectDescription(){}
   //ProjectDescription(const ProjectDescription&);
   ProjectDescription& operator=(ProjectDescription const&);
   void deserializeJson(const Json::Value &);
   
   void clear(void);
   void init(void);
   void createFakeSource(void); // might go to utils...
   void getFromOptionParser(void);
   
   //getters 
   const std::vector<std::string>&  getAllFileNames(void){ return _fileNames; }
   const std::vector<std::string>&  getAllFileNames(void) const { return _fileNames; }
   const std::string& getInputFileName(void){ return _inputFileName; }
   const std::string& getInputFileName(void) const { return _inputFileName; }
   const std::vector<std::string>& getIncludePaths(void){ return _includePaths; }
   const std::vector<std::string>& getIncludePaths(void) const { return _includePaths; }
   const std::string & getOutputFileName(void) { return _outputFileName; }
   const std::string & getOutputFileName(void) const { return _outputFileName; }
   
protected:   
   std::vector<std::string> _fileNames;
   std::vector<std::string> _dirNames;
   std::vector<std::string> _includePaths;
   std::string _inputFileName;
   std::string _outputFileName;
   std::string _workspace;
};

#endif /* _UTBuilder_ProjectDescription_h__ */
