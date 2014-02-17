#ifndef KYAML_UTILS_HH
#define KYAML_UTILS_HH

#include <string>
#include <istream>

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

  bool extract_utf8(std::istream &stream, char32_t &result);
 
  void append_utf8(std::string &str, char32_t ch);
}

#endif // KYAML_UTILS_HH
