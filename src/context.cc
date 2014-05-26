#include "context.hh"
#include "clauses_base.hh"

using namespace kyaml;
using namespace kyaml::clauses;


state_guard::state_guard(context &ctx) :
  d_ctx(ctx),
  d_state(d_ctx.get_state()),
  d_canceled(false)
{}

state_guard::~state_guard()
{
  if(!d_canceled)
    d_ctx.set_state(d_state);
}

stream_guard::stream_guard(context &ctx) :
  d_ctx(ctx),
  d_mark(ctx.stream().mark()),
  d_line(ctx.linenumber()),
  d_canceled(false)
{}

stream_guard::~stream_guard()
{
  if(!d_canceled)
  {
    d_ctx.stream().unwind(d_mark);
    d_ctx.set_linenumber(d_line);
  }
}

void context_guard::release()
{
  d_stream_guard.release();
  d_state_guard.release();
}
