#include "utils.hh"
#include <cstdint>
#include <sstream>

using namespace std;
using namespace kyaml;

namespace
{
  uint8_t inverse(char c)
  {
    if (c >= 'A' && c <= 'Z')
    {
      return c - 'A';
    }
    else if (c >= 'a' && c <= 'z')
    {
      return (c - 'a') + 26;
    }
    else if (c >= '0' && c <= '9')
    {
      return (c - '0') + 52;
    }
    else if (c == '+')
    {
      return 62;
    }
    else if (c == '/')
    {
      return 63;
    }
    return 0;
  }

  size_t from_base64(char const s[4], uint8_t t[3])
  {
    uint8_t c0 = inverse(s[0]);
    uint8_t c1 = inverse(s[1]);
    uint8_t c2 = inverse(s[2]);
    uint8_t c3 = inverse(s[3]);

    t[0] = (c0 << 2) | (c1 >> 4);
    t[1] = ((c1 & 0x0f) << 4) | (c2 >> 2);
    t[2] = ((c2 & 0x03) << 6) | (c3 & 0x3f);

    if (s[2] == '=')
      return 1;
    else if (s[3] == '=')
      return 2;
    else
      return 3;
  }
}

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


bool kyaml::is_valid_utf8(string const &str)
{
  if(str.empty())
    return true;

  uint8_t c = str[0];
  size_t count = nr_utf8bytes(c);
  if(str.size() < count)
    return false;

  for(size_t i = 1; i < count; ++i)
    if(!is_continuation_byte(str[i]))
      return false;

  return true;
}


bool kyaml::is_valid_utf8(char32_t c)
{
  string str;
  append_utf8(str, c);
  return is_valid_utf8(str);
}

bool kyaml::decode_base64(string const &source, vector<uint8_t> &target)
{
  if((source.size() & 3) != 0)
    return false;

  target.reserve(source.size() * 3 / 4);

  for(string::size_type i = 0; i < source.size(); i += 4)
  {
    char const *s = source.data() + i;
    uint8_t t[3];
    size_t l = from_base64(s, t);
    target.insert(target.end(), t, t + l);
  }

  return true;
}
