#include "utils.hh"
#include <cstdint>
#include <sstream>

using namespace std;
using namespace kyaml;

bool kyaml::extract_utf8(istream &stream, char32_t &result)
{
  uint8_t c;
  if(stream.read(reinterpret_cast<char*>(&c), 1))
  {
    result = c;
    if(is_lead_byte(c))
    {
      for(size_t i = 0; i < 3; ++i)
      {
        if(stream.read(reinterpret_cast<char*>(&c), 1))
        {
          result <<= 8;
          result |= c;
          if(!is_continuation_byte(c))
            break;
        }
        else
          break;
      }
    }

    return true;
  }
  return false;
}

bool kyaml::extract_utf8(string const &str, char32_t &result)
{
  stringstream stream(str);
  return extract_utf8(stream, result);
}

void kyaml::append_utf8(string &str, char32_t ch)
{
  uint8_t byte;
  if(ch & 0xff000000)
  {
    byte = ch >> 24;
    str.append(1, byte);
  }
  if(ch & 0xffff0000)
  {
    byte = ch >> 16;
    str.append(1, byte);
  }
  if(ch & 0xffffff00)
  {
    byte = ch >> 8;
    str.append(1, byte);
  }
  byte = ch & 0x000000ff;
  str.append(1, byte);
}


size_t kyaml::nr_utf8bytes(uint8_t c)
{
  if(c < 0x80)
    return 1;
  else if(c < 0xe0)
    return 2;
  else if(c < 0xf0)
    return 3;
  else if(c < 0xf8)
    return 4;
  else if(c < 0xfc)
    return 5;
  else if(c < 0xfe)
    return 6;
  else
    return 0;
}

string invalid_utf8::construct(string const &msg, string const &sequence)
{
  stringstream str;
  str.setf(ios::hex);
  str << msg << ":";
  for(uint8_t b : sequence)
    str << ' ' << b;

  return str.str();
}
