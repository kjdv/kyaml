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
    void start_sequence() override
    {}

    void end_sequence() override
    {}

    void start_mapping() override
    {}

    void end_mapping() override
    {}

    void add_anchor(std::string const &) override
    {}

    void add_alias(std::string const &) override
    {}

    void add_scalar(std::string const &) override
    {}

    void add_property(std::string const &) override
    {}

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

  int number_of_white(context &ctx, document_builder &builder) // will return max+1 when overflowing
  {
    int count = 0;

    space sp(ctx);
    while(sp.parse(builder) && count <= ctx.indent_level())
      ++count;

    return count;
  }
}

bool indent_clause_eq::parse(document_builder &builder)
{
  indent_builder b;
  int n = number_of_white(ctx(), b);
  if(n == level())
    return true;
  else
    unwind();
  return false;
}

bool internal::indent_clause_ge::parse(document_builder &builder)
{
  if(level())
  {
    indent_builder b;
    int n = number_of_white(ctx(), b);
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
    int n = number_of_white(ctx(), b);
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
    int n = number_of_white(ctx(), b);
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

