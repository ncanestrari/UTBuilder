
#include "optionparser.h"

#include <boost/filesystem.hpp>
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
using boost::filesystem::current_path;
using boost::filesystem::exists;
using boost::filesystem::path;
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
#include "Utils.h"



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




/* OptionParser class method definition */
void OptionParser::createOptionMap(int ac, const char* av[])
{   
   //setup general options
   options_description general("General options");
   general.add_options()
      ("help", "print help message")
      ("version", "print version")
      ;
   
   options_description ci("Set the CI path");
   ci.add_options()
      ("ci", value<string>(), "will set the current directory to arg")
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
      ("functions", value<CommaSeparatedVector>()->multitoken(), "list of functions to test comma separated")
      ("output", value<string>(), "basename of the out files\n\tas in output-mocks.h output-utest.cpp output-mtest.cpp")
      ("example", value<bool>(), "will create an example.json for developer to play with")
      ;
   
   _visible.add(ci).add(json).add(example);
   _all.add(general).add(ci).add(json).add(example);
   
   store(parse_command_line(ac, av, _all), _vm);
   
   //Declare conflicting options
   conflicting_options(_vm, "json", "files");
   conflicting_options(_vm, "json", "dirs");
   conflicting_options(_vm, "json", "functions");
   conflicting_options(_vm, "json", "output");
   conflicting_options(_vm, "json", "example");
   
   // change the current path to the arg or --ci is exists, otherwise raise excpetion
   if( _vm.count("ci") ){
      path p = path(_vm["ci"].as<string>());
      if( !exists(p) ){
         throw logic_error("argument of --ci:\n\n\t" + p.string() + "\n\ndoes not exists\n");
      }
      current_path(p);
   }
   
}


void OptionParser::getFileNames(vector<string> &files) const
{
   //if dirs get the files
   set<string> sources;
   if( _vm.count("dirs") ){
      for( auto dir : _vm["dirs"].as<CommaSeparatedVector>().values ){
         glob(dir+"/*.c", sources);
         glob(dir+"/*.cpp", sources);
      }
      files.insert(files.end(), sources.begin(), sources.end());
   }
   
   //if files 
   if( _vm.count("files") ){
      files.insert( files.end(), _vm["files"].as<CommaSeparatedVector>().values.begin(), _vm["files"].as<CommaSeparatedVector>().values.end() );
   }
   
   if( !( _vm.count("dirs") || _vm.count("files")) ){
      throw logic_error(string("No files or dirs options are provided."));
   }
}


const set<string> OptionParser::getFunctionsToTest() const
{
   const vector<string>& functionsStr = _vm["functions"].as<CommaSeparatedVector>().values;
   return set<string>(functionsStr.begin(),functionsStr.end());
}




