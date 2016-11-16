
#ifndef OPTIONPARSER_H
#define OPTIONPARSER_H

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>
#include <boost/foreach.hpp>
#include <set>
#include <string>
#include <iostream>


#include "Utils.h"

#define VERSION "0.1 beta"

/* Auxillary type to handle list of comma separated string */
class CommaSeparatedVector
{
public:
   // values specified with --mylist will be stored here
   std::vector<std::string> values;

   // Function which validates additional tokens from command line.
   friend std::istream& operator>>(std::istream& in, CommaSeparatedVector &csv);
};



class OptionParser
{
public:
   OptionParser() = default;
   
   OptionParser(const OptionParser& other) : 
   _vm(other._vm), 
   _all(other._all) {}
   
   ~OptionParser(){}

   void createOptionMap(int ac, const char* av[]);
   void printHelp(void) { std::cout << _visible << std::endl; }
   void printVersion(void) { std::cout << VERSION << std::endl; }
   void printAll(void) { std::cout << _all << std::endl; }
   
   bool isHelp(void) { return _vm.count("help"); }
   bool isVersion(void) { return _vm.count("version"); }
   bool isExampleEnabled(void) { return _vm.count("example"); }
   bool isUnitTest(void) { return (_vm.count("files") && _vm["files"].as<CommaSeparatedVector>().values.size() > 1) || _vm.count("dirs"); }
   bool isModuleTest(void) { return !isUnitTest(); }
   bool isFunctionToTestEnabled(void) const { return _vm.count("functions"); }
   bool isJsonFileNameEnabled(void) const { return _vm.count("json"); }
   
//    const std::vector<std::string>& getFunctionsToTest(void) { return _vm["functions"].as<CommaSeparatedVector>().values; }
   const std::string & getOutputName(void) const { return _vm["output"].as<std::string>(); }
   const std::string & getJsonFileName(void) const { return _vm["json"].as<std::string>(); }
   void getFileNames(std::vector<std::string>&) const;
//    void getFunctionsToTest(std::set<std::string>&);
   const std::set<std::string> getFunctionsToTest() const;
   
protected:

   boost::program_options::variables_map _vm;
   boost::program_options::options_description _all;
   boost::program_options::options_description _visible;

};

#endif // OPTIONPARSER_H
