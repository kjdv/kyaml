#include "char_stream.hh"
#include <cassert>

using namespace std;
using namespace kyaml;

bool char_stream::get(char &c)
{
  if(!underflow())
    return false;

  assert(d_pos < d_buffer.size());

  c = d_buffer[d_pos++];
  return true;
}

bool char_stream::peek(char &c)
{
  if(!underflow())
    return false;

  assert(d_pos < d_buffer.size());

  c = d_buffer[d_pos];
  return true;
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
    result.assign(begin, end);
  }

  d_buffer.erase(d_buffer.begin(), d_buffer.begin() + d_pos);
  d_pos = 0;
  d_mark_valid = false;

  return result;
}

bool char_stream::underflow()
{
  while(d_buffer.size() <= d_pos)
  {
    char c;
    if(d_base.read(&c, 1))
      d_buffer.push_back(c);
    else
      return false;
  }

  return true;
}
