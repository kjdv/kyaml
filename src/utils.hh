#ifndef KYAML_UTILS_HH
#define KYAML_UTILS_HH

#include <string>

namespace kyaml
{
  inline bool startswith(std::string const &str, std::string const &prefix)
  {
    return 
      str.size() >= prefix.size() &&
      str.compare(0, prefix.size(), prefix) == 0;
  }

  // returns true if str starts with prefix followed by eof or a space
  inline bool startswith_token(std::string const &str, std::string const &prefix)
  {
    return
      startswith(str, prefix) && 
      (
        str.size() == prefix.size() ||
        (
          !str.empty() &&
          isspace(str.back())
        )
      );
  }
}

#endif // KYAML_UTILS_HH
