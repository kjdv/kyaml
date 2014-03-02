#include "separation_line_clauses.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

separate::separate(context &ctx) :
  clause(ctx),
  d_dispatch(nullptr)
{
  context::blockflow_t bf = ctx.blockflow();

  switch(bf)
  {
  case context::BLOCK_OUT:
  case context::BLOCK_IN:
  case context::FLOW_OUT:
  case context::FLOW_IN:
    d_dispatch = &separate::parse_lines;
    break;
  case context::BLOCK_KEY:
  case context::FLOW_KEY:
    d_dispatch = &separate::parse_in_line;
    break;
  default:
    break;
  }
}

bool separate::parse_lines(document_builder &builder)
{
  separate_in_lines s(ctx());
  return s.parse(builder);
}

bool separate::parse_in_line(document_builder &builder)
{
  separate_in_line s(ctx());
  return s.parse(builder);
}
