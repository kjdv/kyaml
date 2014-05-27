#include "structure_clauses.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

bool internal::start_of_line::parse(document_builder &builder)
{
  char32_t c;

  // TODO: should this be done?
  // while(ctx().stream().peek(c) && (c == '\n' || c == '\r'))
  //   ctx().stream().advance();

  // the last character was a newline 
  if(!ctx().stream().rpeek(c) ||
     (c == '\n' || c == '\r'))
  {
    // the current one is not
    if(ctx().stream().peek(c) &&
       (c != '\n' && c != '\r'))
      return true;
  }

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


bool as_space::parse(document_builder &builder)
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


bool trimmed::parse(document_builder &builder)
{

  internal::and_clause<non_content,
                       internal::one_or_more<empty_line> > delegate(ctx());
  null_builder nb;
  return delegate.parse(nb);
}
