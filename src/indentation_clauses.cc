#include "indentation_clauses.hh"
#include "char_clauses.hh"
#include <iostream>

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

namespace
{
  unsigned number_of_white(context &ctx) // will return max+1 when overflowing
  {
    unsigned count = 0;

    dummy_document_builder b;
    space sp(ctx);
    while(sp.parse(b) && count <= ctx.indent_level())
      ++count;

    return count;
  }
}

bool indent_clause_eq::parse(document_builder &builder)
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

bool internal::indent_clause_ge::parse(document_builder &builder)
{
  if(level())
  {
    unsigned n = number_of_white(ctx());
    if(n >= level())
    {
      unwind();
      advance(level());
      return true;
    }
    else
      unwind();
  }
  return false;
}

bool indent_clause_lt::parse(document_builder &builder)
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

bool indent_clause_le::parse(document_builder &builder)
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

