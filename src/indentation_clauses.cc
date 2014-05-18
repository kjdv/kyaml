#include "indentation_clauses.hh"
#include "char_clauses.hh"
#include <iostream>
#include <cassert>

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

namespace
{
  class indent_builder : public document_builder
  { 
  public:
    void add_atom(char32_t c) override
    {
      append_utf8(d_val, c);
    }

    string value(size_t n) 
    {
      return d_val.substr(0, n);
    }

  private:
    string d_val;
  };

  unsigned number_of_white(context &ctx, document_builder &builder) // will return max+1 when overflowing
  {
    unsigned count = 0;

    space sp(ctx);
    while(sp.parse(builder) && count <= ctx.indent_level())
      ++count;

    return count;
  }
}

bool indent_clause_eq::parse(document_builder &builder)
{
  if(level())
  {
    indent_builder b;
    unsigned n = number_of_white(ctx(), b);
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
    indent_builder b;
    unsigned n = number_of_white(ctx(), b);
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
    indent_builder b;
    unsigned n = number_of_white(ctx(), b);
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
    indent_builder b;
    unsigned n = number_of_white(ctx(), b);
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

