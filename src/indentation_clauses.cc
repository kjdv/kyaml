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
    void start_sequence(context const &ctx) override
    {}

    void end_sequence(context const &ctx) override
    {}

    void start_mapping(context const &ctx) override
    {}

    void end_mapping(context const &ctx) override
    {}

    void add_anchor(context const &ctx, std::string const &) override
    {}

    void add_alias(context const &ctx, std::string const &) override
    {}

    void add_scalar(context const &ctx, std::string const &) override
    {}

    void add_property(context const &ctx, std::string const &) override
    {}

    void add_atom(context const &ctx, char32_t c) override
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
  stream_guard sg(ctx());

  indent_builder b;
  int n = number_of_white(ctx(), b);
  if(n == level())
  {
    sg.release();
    return true;
  }

  return false;
}

bool internal::indent_clause_ge::parse(document_builder &builder)
{
  int n = -1;

  {
    stream_guard sg(ctx());

    indent_builder b;
    n = number_of_white(ctx(), b);
  }

  if(n >= level())
  {
    ctx().stream().advance(level());
    return true;
  }
  return false;
}

bool indent_clause_lt::parse(document_builder &builder)
{
  stream_guard sg(ctx());

  if(higher_level())
  {
    indent_builder b;
    int n = number_of_white(ctx(), b);
    if(n < higher_level())
    {
      d_m = n;
      sg.release();
      return true;
    }
  }
  return false;
}

bool indent_clause_le::parse(document_builder &builder)
{
  stream_guard sg(ctx());

  if(higher_level())
  {
    indent_builder b;
    int n = number_of_white(ctx(), b);
    if(n <= higher_level())
    {
      d_m = n;
      sg.release();
      return true;
    }
  }
  return false;
}

