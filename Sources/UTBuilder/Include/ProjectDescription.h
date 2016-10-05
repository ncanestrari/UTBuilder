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
   void setOutputFileName(const std::string &);
   void setFileNames(const std::vector<std::string> &);
   void setDirNames(const std::vector<std::string> &);
   std::vector<std::string>&  getAllFileNames(void);
   void clear(void);
   void init(void);
   void createFakeSource(void); // might go to utils...
   std::string& getInputFileName(void);
   std::vector<std::string>& getIncludePaths(void);
protected:
   std::string& getPackageFullPath(void);
   std::vector<std::string> _fileNames;
   std::vector<std::string> _dirNames;
   std::vector<std::string> _includePaths;
   std::string _inputFileName;
   std::string _outputFileName;
   std::string _workspace;
};

#endif /* _UTBuilder_ProjectDescription_h__ */