#include "indentation_clauses.hh"
#include "char_clauses.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

namespace
{
  unsigned number_of_white(char_stream &s, unsigned max) // will return max+1 when overflowing
  {
    unsigned count = 0;

    space sp(s);
    while(sp.try_clause() && count <= max)
      ++count;

    return count;
  }
}

bool indent_clause_eq::try_clause()
{
  if(d_n)
  {
    unsigned n = number_of_white(stream(), d_n);
    if(n == d_n)
      return true;
    else
      unwind();
  }
  return false;
}

bool indent_clause_lt::try_clause()
{
  if(d_n)
  {
    unsigned n = number_of_white(stream(), d_n);
    if(n < d_n)
    {
      d_m = n;
      return true;
    }
    else
      unwind();
  }
  return false;
}

bool indent_clause_le::try_clause()
{
  if(d_n)
  {
    unsigned n = number_of_white(stream(), d_n);
    if(n <= d_n)
    {
      d_m = n;
      return true;
    }
    else
      unwind();
  }
  return false;
}
