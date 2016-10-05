
#ifndef OPTIONPARSER_H
#define OPTIONPARSER_H

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <set>
#include <string>

#include "singleton.h"


class OptionParser : public Singleton<OptionParser>
{
public:
   OptionParser(void) {}
   
   OptionParser(const OptionParser& other) : 
   _vm(other._vm), 
   _all(other._all) {}
   
   ~OptionParser(){}

   bool createOptionMap(int ac, const char* av[]);
   bool isExampleEnabled(void);
   bool isUnitTest(void);
   bool isModuleTest(void);
   const std::string & getOutputName(void);
   void getFileNames(std::vector<std::string> & );
   const std::string & getJsonFileName(void);
   void getCommercialCode(std::string &);
   void getPackage(std::string &);
   
   
protected:

   boost::program_options::variables_map _vm;
   boost::program_options::options_description _all;
   const std::string& getFirstAvailableFile(void); 

};

#endif // OPTIONPARSER_H
