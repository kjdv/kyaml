#ifndef KYAML_UTILS_HH
#define KYAML_UTILS_HH

#include <string>
#include <istream>

namespace kyaml
{
  bool extract_utf8(std::istream &stream, char32_t &result);
 
  void append_utf8(std::string &str, char32_t ch);
  // just a small helper to support uniform appending
  inline void append_utf8(std::string &str, std::string const &s)
  {
    str.append(s);
  }
}

#endif // KYAML_UTILS_HH
