#include "indentation_clauses.hh"
#include "char_clauses.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

namespace
{
  unsigned number_of_white(context &ctx) // will return max+1 when overflowing
  {
    unsigned count = 0;

    space sp(ctx);
    while(sp.try_clause() && count <= ctx.indent_level())
      ++count;

    return count;
  }
}

bool indent_clause_eq::try_clause()
{
  if(level())
  {
    unsigned n = number_of_white(ctx());
    if(n == level())
      return true;
    else
      unwind();
  }
  return false;
}

bool indent_clause_lt::try_clause()
{
  if(higher_level())
  {
    unsigned n = number_of_white(ctx());
    if(n < higher_level())
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
  if(higher_level())
  {
    unsigned n = number_of_white(ctx());
    if(n <= higher_level())
    {
      d_m = n;
      return true;
    }
    else
      unwind();
  }
  return false;
}
