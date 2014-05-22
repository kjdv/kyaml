#include "char_stream.hh"
#include "utils.hh"
#include <cassert>

using namespace std;
using namespace kyaml;

bool char_stream::get(char_t &c)
{
  if(!underflow())
    return false;

  assert(d_pos < d_buffer.size());

  c = d_buffer[d_pos++];
  return true;
}

bool char_stream::peek(char_t &c)
{
  if(!underflow())
    return false;

  assert(d_pos < d_buffer.size());

  c = d_buffer[d_pos];
  return true;
}

bool char_stream::rpeek(char_t &c)
{
  if(d_pos)
  {
    c = d_buffer[d_pos - 1];
    return true;
  }
  return false;
}

void char_stream::advance(size_t n)
{
  d_pos += n;
  underflow();
}

char_stream::mark_t char_stream::mark() const
{
  d_mark_valid = true;
  return d_pos;
}

void char_stream::unwind(mark_t m)
{
  assert(d_mark_valid);
  assert(m <= d_buffer.size());

  d_pos = m;
}

string char_stream::consume(mark_t m)
{
  string result;
  if(m < d_pos)
  {
    auto begin = d_buffer.begin() + m;
    auto end = d_buffer.begin() + d_pos;

    for(auto it = begin; it != end; ++it)
      append_utf8(result, *it);
  }

  ignore();
  return result;
}

size_t char_stream::indent_level(size_t hint) const
{
  if(hint >= d_pos)
    return 0;

  size_t pos = d_pos - hint - 1;
  while(pos < d_buffer.size()) // rely on overflow for termination
  {
    char_t c = d_buffer[pos];
    if(c == '\n' || c == '\r')
      break;
    --pos;
  }

  return d_pos - pos - 1;
}

void char_stream::ignore()
{
  d_buffer.erase(d_buffer.begin(), d_buffer.begin() + d_pos);
  d_pos = 0;
  d_mark_valid = false;
}

bool char_stream::underflow()
{
  while(d_buffer.size() <= d_pos)
  {
    char32_t c;
    if(extract_utf8(d_base, c))
      d_buffer.push_back(c);
    else
      return false;
  }

  return true;
}
