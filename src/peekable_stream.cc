#include "peekable_stream.hh"
#include <algorithm>

using namespace std;
using namespace kyaml;

int peekable_stream::getc()
{
  if(!d_buffer.empty())
  {
    int result = d_buffer.front();
    d_buffer.pop_front();
    return result;
  }
  else
  {
    return d_stream.getc();
  }
}

string peekable_stream::peek(size_t n)
{
  string result;
  result.reserve(n);

  if(n && !d_buffer.empty())
  {
    size_t from_buffer = std::min<size_t>(n, d_buffer.size());
    result.assign(d_buffer.begin(), d_buffer.begin() + from_buffer);
    n -= from_buffer;
  }
  
  int c;
  while(n && (c = d_stream.getc() >= 0))
  {
    d_buffer.push_back(c);
    result += c;
    --n;
  }

  return result;
}

string peekable_stream::read(size_t n)
{
  // can be optimized
  string result = peek(n);
  ignore(n);
  return result;
}

void peekable_stream::ignore(size_t n)
{
  size_t from_buffer = std::min<size_t>(n, d_buffer.size());
  if(from_buffer)
  {
    d_buffer.erase(d_buffer.begin(), d_buffer.begin() + from_buffer);
    n -= from_buffer;
  }

  if(n)
  {
    d_stream.ignore(n);
  }
}
