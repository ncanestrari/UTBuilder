
#ifndef __GLOBBER_H__
#define __GLOBBER_H__

#include <boost/filesystem.hpp>

#include <set>
#include <string>

void glob(const std::string&                  pattern,
          std::set<std::string>&              ret);

void glob(const std::string&                  pattern,
          std::set<boost::filesystem::path>&  ret);

#endif /* !__GLOBBER_H__ */

