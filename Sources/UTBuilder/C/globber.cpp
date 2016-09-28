
#include "globber.h"

extern "C" {
#include <glob.h>
}

using std::string;
using std::vector;

vector<string> glob(const string &pat)
{
   glob_t            glob_result;
   vector<string>    ret;

   glob(pat.c_str(), 0, NULL, &glob_result);
   for (unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
      ret.push_back(string(glob_result.gl_pathv[i]));
   }

   globfree(&glob_result);

   return ret;
}

