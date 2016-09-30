
#ifndef OPTIONPARSER_H
#define OPTIONPARSER_H

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <set>
#include <string>


class OptionParser
{
public:
   OptionParser(int ac, char* av[]) : 
   _ac(ac), 
   _av(av) {}
   
   OptionParser(const OptionParser& other) : 
   _ac(other._ac), 
   _av(other._av), 
   _vm(other._vm), 
   _all(other._all) {}
   
   ~OptionParser(){}

   bool createOptionMap(void);
   bool isExampleEnabled(void);
   bool isUnitTest(void);
   bool isModuleTest(void);
   const std::string & getOutputName(void) const;
   void getFileNames(std::vector<std::string> & ) const;
   const std::string & getJsonFileName(void) const;
   
protected:
   int    _ac;
   char **_av;
   boost::program_options::variables_map _vm;
   boost::program_options::options_description _all;
   

};

#endif // OPTIONPARSER_H
