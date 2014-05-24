#include "structure_clauses.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

bool internal::start_of_line::parse(document_builder &builder)
{
  char32_t c;

  // TODO: should this be done?
  while(stream().peek(c) && (c == '\n' || c == '\r'))
    advance();

  // the last character was a newline
  if(!stream().rpeek(c) ||
     (c == '\n' || c == '\r'))
  {
    // the current one is not
    if(stream().peek(c) &&
       (c != '\n' && c != '\r'))
      return true;
  }

  unwind();
  return false;
}

line_prefix::line_prefix(context &ctx) :
  clause(ctx),
  d_dispatch(nullptr)
{
  context::blockflow_t bf = ctx.blockflow();

  switch(bf)
  {
  case context::BLOCK_OUT:
  case context::BLOCK_IN:
    d_dispatch = &line_prefix::parse_block;
    break;
  case context::FLOW_IN:
  case context::FLOW_OUT:
    d_dispatch = &line_prefix::parse_flow;
    break;
  default:
    break;
  }
}

bool line_prefix::parse_block(document_builder &builder)
{
  block_line_prefix bl(ctx());
  return bl.parse(builder);
}

bool line_prefix::parse_flow(document_builder &builder)
{
  flow_line_prefix fl(ctx());
  return fl.parse(builder);
}


bool kyaml::clauses::as_space::parse(kyaml::document_builder &builder)
{
  line_break lb(ctx());
  null_builder nb;
  if(lb.parse(nb))
  {
    builder.add_atom(' ');
    return true;
  }
  return false;
}
