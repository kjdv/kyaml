#include "flow_clauses.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

bool in_flow::parse(document_builder &builder)
{
  switch(ctx().blockflow())
  {
  case context::FLOW_OUT:
    ctx().set_blockflow(context::FLOW_IN);
  case context::FLOW_IN:
    return true;

  case context::BLOCK_KEY:
    ctx().set_blockflow(context::FLOW_KEY);
  case context::FLOW_KEY:
    return true;

  default:
    return false;
  }
}
