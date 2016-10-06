
#include "optionparser.h"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>
#include <boost/foreach.hpp>
using boost::any;
using boost::any_cast;
using boost::char_separator;
using boost::tokenizer;
using boost::program_options::notify;
using boost::program_options::options_description;
using boost::program_options::parse_command_line;
using boost::program_options::store;
using boost::program_options::value;
using boost::program_options::variables_map;

#include <exception>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
using std::back_inserter;
using std::copy;
using std::cout;
using std::logic_error;
using std::istream;
using std::set;
using std::string;
using std::vector;


#include "globber.h"
#include "utils.h"

/* Auxillary type to handle list of comma separated string */
class CommaSeparatedVector
{
public:
   // values specified with --mylist will be stored here
   vector<string> values;

   // Function which validates additional tokens from command line.
   friend istream& operator>>(istream& in, CommaSeparatedVector &csv);
};

void tokenize(const string& str, vector<string>& tokens, const string& delimiters = ",")
{
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);

  // Find first non-delimiter.
  string::size_type pos = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos) {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));

    // Skip delimiters.
    lastPos = str.find_first_not_of(delimiters, pos);

    // Find next non-delimiter.
    pos = str.find_first_of(delimiters, lastPos);
  }
}

istream& operator>>(istream& in, CommaSeparatedVector &csv)
{
    string token;
    in >> token;

    tokenize(token, csv.values);

    return in;
}



/* Auxillary Function to handle conflicting options */
static void conflicting_options(const variables_map &vm, const string &opt1, const string &opt2)
{
   if( (vm.count(opt1)) && (!vm[opt1].defaulted()) && 
       (vm.count(opt2)) && (!vm[opt2].defaulted()) ){
      throw logic_error(string("Conflicting options '") + opt1 + "' and '" + opt2 + "'.");
   }
}


/* Auxillary Function */



/* OptionParser class method definition */
bool OptionParser::createOptionMap(int ac, const char* av[])
{   
   //setup general options
   options_description general("General options");
   
   const char help_message[] = 
   "usage:\
   \n   UTBuilder --json=<path/jsonfilename>\
   \nTo generate the source files. Alternatively call\
   \n   UTBuilder --example --files=<path1/source1>.c,... [--dirs=<path1>,...] --output=<path/output.json>\
   \nTo create an json example file\n\n";
   
   general.add_options()
      ("help", help_message)
      ("version", "0.1beta")
      ;
   
   //setup json options
   options_description json("Json options");
   json.add_options()
      ("json", value<string>(), "json file to be loaded")
      ;
   
   options_description example("create Json example");
   example.add_options()
      ("files", value<CommaSeparatedVector>()->multitoken(), "list of files comma separated")
      ("dirs", value<CommaSeparatedVector>()->multitoken(), "list of directories comma separated")
      ("output", value<string>(), "basename of the out files\n\tas in output-mocks.h output-utest.cpp output-mtest.cpp")
      ("example", value<bool>(), "will create an example.json for developer to play with")
      ;
   
   options_description visible;
   visible.add(example).add(json);
   _all.add(general).add(json).add(example);
   
   store(parse_command_line(ac, av, _all), _vm);
   
   //Declare conflicting options
   conflicting_options(_vm, "json", "files");
   conflicting_options(_vm, "json", "dirs");
   conflicting_options(_vm, "json", "output");
   conflicting_options(_vm, "json", "example");
   
   //if help is called 
   if( _vm.count("help") ){
      cout << visible;
      return false;
   }
   
   if( _vm.count("version") ){
      cout << "0.1 Beta\n";
      return false;
   }
   
   return true;
}

bool OptionParser::isExampleEnabled(void)
{
   return _vm.count("example");
}

bool OptionParser::isUnitTest(void)
{
   //TODO we are discarding the possibility of one file in a dir
   bool ans = (_vm.count("files") && _vm["files"].as<CommaSeparatedVector>().values.size() > 1) || _vm.count("dirs"); 
   return ans;
}

bool OptionParser::isModuleTest(void)
{
   return !isUnitTest();
}

const string & OptionParser::getOutputName(void)
{   
   if( _vm.count("output") ){
      return _vm["output"].as<string>();
   } else {
      throw logic_error(string("No output name provided."));
   }
}

void OptionParser::getFileNames(vector<string> & files)
{
   //if dirs get the files
   if( _vm.count("dirs") ){
      for( auto dir : _vm["dirs"].as<CommaSeparatedVector>().values ){
         vector<string> globs = glob(dir+"/*.c");
         files.insert(files.end(), globs.begin(), globs.end());
         globs = glob(dir+"/*.cpp");
         files.insert(files.end(), globs.begin(), globs.end());
      }
   }
   //if files 
   if( _vm.count("files") ){
      files.insert( files.end(), _vm["files"].as<CommaSeparatedVector>().values.begin(), _vm["files"].as<CommaSeparatedVector>().values.end() );
   }
   
   if( !( _vm.count("dirs") || _vm.count("files")) ){
      cout << _all;
      throw logic_error(string("No files or dirs options are provided."));
   }
}

const string & OptionParser::getJsonFileName(void)
{
   if( _vm.count("json") ){
      return _vm["json"].as<string>();
   } else {
      throw logic_error(string("No json file provided."));
   }
}

const string& OptionParser::getFirstAvailableFile(void)
{
   if( isExampleEnabled() ){
      //grab the ci from the first file | the first dir
      if( _vm.count("files") && _vm["files"].as<CommaSeparatedVector>().values.size() >= 1 ){
         return _vm["files"].as<CommaSeparatedVector>().values[0];
      }
      if( _vm.count("dirs") && _vm["dirs"].as<CommaSeparatedVector>().values.size() >= 1 ){
         return _vm["dirs"].as<CommaSeparatedVector>().values[0];
      }
   } else {
      //grab the ci from the json file
      if( _vm.count("json") ){
         return _vm["json"].as<string>();
      }
   }
   throw logic_error(string("not able to find any information!"));
}

void OptionParser::getCommercialCode(string &ci)
{
   ci = utils::extractCommercialCodePath( getFirstAvailableFile() );
}

void OptionParser::getPackage(string &package)
{
   package = utils::extractPackagePath( getFirstAvailableFile() );
}

