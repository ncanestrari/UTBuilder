
#include "globber.h"

extern "C" {
#include <glob.h>
}

using std::set;
using std::string;
using boost::filesystem::path;
using std::cout;
using std::endl;

void glob(const string&  pattern,
          set<string>&   ret)
{
   glob_t            glob_result;

   glob(pattern.c_str(), 0, NULL, &glob_result);
   for (unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
      ret.insert(string(glob_result.gl_pathv[i]));
   }
   globfree(&glob_result);
}


void glob(const string&  pattern,
          set<path>&     ret)
{
   set<string> listMatches;
   
   glob(pattern, listMatches);
   for( const string &match : listMatches ){
      ret.insert(path(match));
   }
}
