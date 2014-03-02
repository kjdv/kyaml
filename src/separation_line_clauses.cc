#ifdef COMPILE_GUARD

#include "separation_line_clauses.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

separate::separate(context &ctx) :
  void_clause(ctx),
  d_dispatch(nullptr)
{
  context::blockflow_t bf = ctx.blockflow();

  switch(bf)
  {
  case context::BLOCK_OUT:
  case context::BLOCK_IN:
  case context::FLOW_OUT:
  case context::FLOW_IN:
    d_dispatch = &separate::try_lines;
    break;
  case context::BLOCK_KEY:
  case context::FLOW_KEY:
    d_dispatch = &separate::try_in_line;
    break;
  default:
    break;
  }
}

bool separate::try_lines()
{
  separate_in_lines s(ctx());
  return s.try_clause();
}

bool separate::try_in_line()
{
  separate_in_line s(ctx());
  return s.try_clause();
}

#endif // COMPILE_GUARD
