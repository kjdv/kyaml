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
    char c;
    return d_stream.get(c) ? c : -1;
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
  
  char c;
  while(n && d_stream.get(c))
  {
    d_buffer.push_back(c);
    result += c;
    --n;
  }

  return result;
}

int peekable_stream::peekc()
{
  if(!d_buffer.empty())
  {
    return d_buffer.front();
  }
  else
  {
    char c;
    if(d_stream.get(c))
    {
      d_buffer.push_back(c);
      return c;
    }
    else
    {
      return -1;
    }
  }
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

void peekable_stream::ignore_until(char d)
{
  int c;
  do
  {
    c = getc();
  } while(c >= 0 && c != d);  
}

void peekable_stream::putback(char c)
{
  d_buffer.push_front(c);
}

void peekable_stream::putback(std::string const &s)
{
  for(auto it = s.rbegin(); it != s.rend(); ++it)
    d_buffer.push_front(*it);
}
